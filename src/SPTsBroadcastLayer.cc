// -*- C++ -*-
//
//  This file is part of CBCBSIM, the Combined Broadcast Content-Based
//  (CBCB) routing simulation.
//
//  Authors: See the file AUTHORS for full details. 
//
//  Copyright (C) 2002-2004 University of Colorado
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
//  USA, or send email to serl@cs.colorado.edu.
//
// $Id: SPTsBroadcastLayer.cc,v 1.1 2004/03/19 17:04:47 carzanig Exp $
//
#include <map>
#include <list>
#include <string>
#include <limits>
#include <stdio.h>
#include "cbcbsim/SPTsBroadcastLayer.h"

using namespace cbcbsim;
using namespace ssim;
using std::list;
using std::map;
using std::numeric_limits;
using std::pair;
using std::set;
using std::string;

typedef int nid_t;

static const nid_t NIL_NID = -1;

typedef double Dist;
static const Dist INFINITE_DISTANCE = numeric_limits<Dist>::max();

//
// what I do here is construct a graph data structure that is
// (obviously) isomorphic to the one entered by the user, but that
// uses sequential integer identifiers for nodes.  I do that in order
// to be able to use a simple predecessor matrix.
//

//
// this is an element of the adjacency list for node descriptors (see
// below)
//
struct Adj {
    const nid_t nid;
    const double len;
    const Adj * next;

    Adj(nid_t n, double l, const Adj * nxt): nid(n), len(l), next(nxt) {};
};

//
// NDescr is a node descriptor.  It stores the mapping between
// ProcessId and node id, the position (index in array) of this node
// in the heap used by Dijkstra, and the adjacency list (actually a
// map from node identifiers into distances)
//
struct NDescr {
    ProcessId pid;		// user-specified node identifier
    nid_t nid;			// (sequential) identifier within the graph 
    int heap_pos;		// position within the min_queue heap
				// (-1 if not there)
    const Adj * adj;		// adjacency list

    NDescr() : pid(NULL_PROCESSID), nid(NIL_NID), heap_pos(-1), adj(0) {};

    void add_adj(nid_t n, double len) { 
	for(const Adj * tmp = adj; tmp != 0; tmp = tmp->next) 
	    if (tmp->nid == n) return;
	adj = new Adj(n, len, adj);    
    }

    void cleanup() {
	const Adj * tmp;
	while(adj != 0) {
	    tmp = adj;
	    adj = adj->next;
	    delete tmp;
	}
    }

    void init(ProcessId p, nid_t n) {
	pid = p; 
	nid = n; 
	heap_pos = -1;
	adj = 0;
    };
};
  
//
// NTable is an array of node descriptors indexed by the node id.
// NTable also maintains a reverse map, used to get the id
// corresponding to the user-specified ProcessId.
//
class NTable {
public:
    NTable(size_t s) : sz(0) {
	nodes = new NDescr[s]();
    }
    ~NTable() {
	while(sz > 0)
	    nodes[--sz].cleanup();

	delete(nodes);
    }
    
    void cleanup(){
      int i=0;
      while( i < sz){
	    nodes[i].cleanup();
            i++;
      }
      printf("size: %d\n", sz);
    }
    
    size_t add_node(ProcessId p) {
	pair<rmap_t::iterator, bool> pib;
	pib = reverse_map.insert(rmap_t::value_type(p, sz));
	if (pib.second) {
	    nodes[sz].init(p, sz);
	    ++sz;
	}
	return  (*pib.first).second;
    }

    void add_edge(ProcessId p, ProcessId q, double len) {
	NDescr & pd = nodes[add_node(p)];
	NDescr & qd = nodes[add_node(q)];
	pd.add_adj(qd.nid, len);
	qd.add_adj(pd.nid, len);
    }

    NDescr &	operator[](size_t pos) { return nodes[pos]; }
    int		size() const { return sz; }

    typedef NDescr * iterator;
    iterator	begin() { return nodes; };
    iterator	end() { return &nodes[sz]; };

private:
    unsigned sz;
    NDescr * nodes;
    typedef map<ProcessId, size_t> rmap_t;
    rmap_t reverse_map;
};


//
// PLink represents an element of the predecessor matrix.  A PLink in
// position (x,y) stores the identifier of the (unicast) next-hop node
// that, starting from y, is on the shortest path to x.  The distance
// is the total (shortest) distance between x and y.
//
struct PLink {
    nid_t prev;
    Dist dist;

    PLink(): prev(NIL_NID), dist(INFINITE_DISTANCE) {};
};

//
// predecessor table: a square matrix of PLink structs
//
class PTable {
public:
    PTable(size_t s): size(s), x(s), y(s) {
	T = new PLink[s * s]();
    };

    PTable(size_t s1,size_t s2): size(s1), x(s1), y(s2) {
	T = new PLink[s1 * s2]();
    };
    
    ~PTable() {
	delete (T);
    };
    
    size_t r_size(){return size;}
    size_t r_x(){return x;}
    size_t r_y(){return y;}
    
    
    PLink & predecessor(size_t x, size_t y) {
	return T[x*size + y];
    }

    PLink & operator()(size_t x, size_t y) {
	return T[x*size + y];
    }

private:
    size_t size;
    size_t x;
    size_t y;
    PLink * T;
};

//
// this is an implementation of a binary heap taken pretty much
// directly from Chapter 6 of CLRS's Intro to Algorithms 2nd Ed.
//
class min_ndescr_heap {
public:
    struct element {
	Dist dist;
	NDescr * descr;

	element(): dist(INFINITE_DISTANCE), descr(0) {};
	element(Dist k, NDescr * d): dist(k), descr(d) {};
	element(const element & x): dist(x.dist), descr(x.descr) {};
	element & operator = (const element & x) {
	    dist = x.dist;
	    descr = x.descr;
	    return *this;
	}
    };

    min_ndescr_heap(int s): sz(0) { A = new element[s](); };
    ~min_ndescr_heap() { delete(A); };
    unsigned	size() { return sz; }

    element pop_first() {
	// ASSERT(heap_size > 1)
	element res = A[0];
	if (sz > 1) {
	    --sz;
	    move_element(A[sz], 0);
	    heapify(0);
	} else {
	    --sz;
	}
	res.descr->heap_pos = -1;
	return res;
    }

    void decrease_dist(int i, Dist k) {
	if (k < A[i].dist) {
	    A[i].dist = k;
	    decrease_dist(i);
	}
    }

    void insert(Dist k, NDescr * d) {
	A[sz].dist = k;
	A[sz].descr = d;
	d->heap_pos = sz;
	decrease_dist(sz++);
    }

 private:
    unsigned sz;
    element * A;

    int parent(int i) { return (i - 1)/2; }
    int left(int i) { return 2*i + 1; }
    int right(int i) { return 2*i + 2; }
    
    void move_element(const element & e, int pos) {
	A[pos] = e;
	A[pos].descr->heap_pos = pos;
    }

    void swap_elements(int x, int y) {
	element tmp = A[x];
	A[x] = A[y];
	A[y] = tmp;
	A[x].descr->heap_pos = x;
	A[y].descr->heap_pos = y;
    }

    void heapify(unsigned i);
    void decrease_dist(int i);
};

void min_ndescr_heap::heapify(unsigned i) {
    unsigned l = left(i);
    unsigned r = right(i);
    unsigned smallest;
    if (l < sz && A[l].dist < A[i].dist) {
	smallest = l;
    } else {
	smallest = i;
    }
    if (r < sz && A[r].dist < A[smallest].dist) {
	smallest = r;
    }
    if (smallest != i) {
	swap_elements(i, smallest);
	heapify(smallest);
    }
}

void min_ndescr_heap::decrease_dist(int i) {
    while(i > 0 && A[i].dist < A[parent(i)].dist) {
	swap_elements(i, parent(i));
	i = parent(i);
    }
}

void SPTsBroadcastLayer::addLink(ProcessId from, ProcessId to, double length) {
    input.push_back(Edge(from, to, length));
    nodes.insert(from);
    nodes.insert(to);
}

void SPTsBroadcastLayer::addNode(ProcessId p) {
    input.push_back(Edge(p, p, 0));
    nodes.insert(p);
}

void dijkstra(NTable & N, PTable & P, nid_t curr_root) {
    //
    // Dijkstra
    //
    min_ndescr_heap Q(N.size());
    P(curr_root, curr_root).prev = curr_root;
    P(curr_root, curr_root).dist = 0;
    for(NTable::iterator nj = N.begin(); nj != N.end(); ++nj) {
	Q.insert(((*nj).nid == curr_root) ? 0 : INFINITE_DISTANCE, &(*nj));
    }
    while(Q.size() > 0) {
	min_ndescr_heap::element lower = Q.pop_first();
	for(const Adj * ai = lower.descr->adj; ai != 0; ai = ai->next) {
	    PLink & alink = P(curr_root, ai->nid);
	    // relaxation
	    if (alink.dist > lower.dist + ai->len) {
		alink.prev = lower.descr->nid;
		alink.dist = lower.dist + ai->len;
		Q.decrease_dist(N[ai->nid].heap_pos, alink.dist);
	    }
	}
    }
}

list<SPTsBroadcastLayer::Edge> dijkstra(NTable & N, nid_t curr_root, nid_t dest) {
    //
    // Dijkstra u,v
    //
    list<SPTsBroadcastLayer::Edge> new_path;
    PTable P(N.size());
    min_ndescr_heap Q(N.size());
    P(curr_root, curr_root).prev = curr_root;
    P(curr_root, curr_root).dist = 0;
//     printf("root: %d\n", curr_root);
    for(NTable::iterator nj = N.begin(); nj != N.end(); ++nj) {
	Q.insert(((*nj).nid == curr_root) ? 0 : INFINITE_DISTANCE, &(*nj));
    }
    while(Q.size() > 0) 
    {
	min_ndescr_heap::element lower = Q.pop_first();
 	if(lower.descr->nid == dest)
	{
	  if (lower.dist != INFINITE_DISTANCE)
	  {
// 	    printf("Path: %d",dest);
	    nid_t node = dest;
	    nid_t next_node;
	    while(node != curr_root)
	    {
	      next_node = P(curr_root,node).prev;
	      if (next_node == node)
	      {
// 		printf("No path");
		break; 
	      }
// 	      printf("->%d",next_node);
	      new_path.push_back(SPTsBroadcastLayer::Edge(node,next_node,0));
	      
	      node=next_node;
//      	      printf("assigned2 root %d \n",curr_root);

	    }
	   
// 	    printf("\n");  
	  }
	  else
	  {
	    return new_path;
	  }
	  //return a bidirectional path between the two nodes.
          return new_path;
	}
	for(const Adj * ai = lower.descr->adj; ai != 0; ai = ai->next) 
	{
	  PLink & alink = P(curr_root, ai->nid);
// 	  printf("Analizing: %d dist:%e\n",lower.descr->nid,lower.dist);
// 	  printf("root: %d nexthop: %d length:%e\n",curr_root, ai->nid, ai->len);
	  if (alink.dist > lower.dist + ai->len) 
	  {
		  alink.prev = lower.descr->nid;
		  alink.dist = lower.dist + ai->len;
		  Q.decrease_dist(N[ai->nid].heap_pos, alink.dist);
	  }
          
	}
	
    }
    return new_path;
}

void SPTsBroadcastLayer::computePaths() {
    NTable N(nodes.size());
    //nodes.clear();
    for(list<Edge>::const_iterator i = input.begin(); i != input.end(); ++i)
    {
	N.add_edge((*i).from, (*i).to, (*i).len);
    }
    //input.clear();
    PTable P(N.size()); // predecessor table



    for(NTable::iterator ni = N.begin(); ni != N.end(); ++ni) 
    {
	dijkstra(N, P, (*ni).nid);
      
    }

    for(nid_t i = 0; i < N.size(); ++i) {
	BLink & T_ii = paths[N[i].pid][N[i].pid];
	T_ii.prev = N[i].pid;
	T_ii.hop_count = 0;

	for(nid_t j = i + 1; j < N.size(); ++j) {
	    nid_t prev_ij, prev_ji;
	    int hop_count;
	    //
	    // this is the trick that gives us all-pairs path
	    // symmetry: we essentially compute the T_ji predecessor
	    // from the path from j to i defined by T_i
	    //
	    prev_ij = P(i, j).prev;
	    if (prev_ij != NIL_NID) {
		hop_count = 1;
		prev_ji = j;
		while(P(i, prev_ji).prev != i) {
		    prev_ji = P(i, prev_ji).prev;
		    ++hop_count;
		}

		B & T_i = paths[N[i].pid];
		B & T_j = paths[N[j].pid];
		BLink & T_ij = T_i[N[j].pid];
		BLink & T_ji = T_j[N[i].pid];

		T_ij.prev = N[prev_ij].pid;
		T_ij.hop_count = hop_count;
		T_i[N[prev_ij].pid].next.insert(N[j].pid);

		T_ji.prev = N[prev_ji].pid;
		T_ji.hop_count = hop_count;
		T_j[N[prev_ji].pid].next.insert(N[i].pid);
	    }
	}
    }
    
//     for (int i = 0; i<N.size();++i)
//     {
//       for (int j = 0; j<N.size();++j)
//       {
// 	if (i != j)
// 	  printf(" i:%d j:%d   pred: %d\n",i,j,P(i,j).prev);
//       }     
//     }
    for (int i = 0; i<N.size();++i)
    {
      for (int j = i; j<N.size();++j)
      {
	if (i != j) //Not interested in multipths from a node to itself!!! 
	{
	  std::list<Edge> explored_paths;
	  Paths::const_iterator pi = paths.find(i);
// 	  printf("Path %d <-> %d\n",i,j);
	  B::const_iterator bi;
	  bi = (*pi).second.find(j);
	  
	  if (bi != (*pi).second.end())
	  {
	    ProcessId from=j;
	    ProcessId to = (*bi).second.prev;
	    if (to!=-1)
	    {
  // 	    printf("inserting from: %d to: %d and vv\n", from,to);
  // 	    path.push_back(Edge(from, to, 0));
	      while((from != i))
	      {
		bi = (*pi).second.find(from);
		to = (*bi).second.prev;
		  //printf("inserting from: %d to: %d and vv\n", from,to);
		explored_paths.push_back(Edge(from, to, 0));
		from = to;
	      }
	    }
	  }
	  unsigned int path_number=2;
	  computeAlternativePaths(explored_paths,i,j,path_number);
	}
      }
    }
//	PRINTING ALTERNATIVE PATHS    
//     printf("Alternative Paths\n\n");
//     for(std::multimap<std::pair<std::pair<ssim::ProcessId,ssim::ProcessId>,unsigned int>, BOneWay>::iterator it_print = alternative_paths.begin(); it_print != alternative_paths.end(); it_print++ )
//     {
//       printf("from %d to %d  number: %d \n ",(*it_print).first.first.first,(*it_print).first.first.second,(*it_print).first.second);
//       ProcessId node = (*it_print).first.first.first;
//       std::map<ssim::ProcessId,BLinkOneWay>::iterator it_print_path_2;
//       for(std::map<ssim::ProcessId,BLinkOneWay>::iterator it_print_path = (*it_print).second.begin(); it_print_path != (*it_print).second.end();it_print_path++)
//       {
// 	      printf("node %d next %d\n",(*it_print_path).first,(*it_print_path).second.next);
//       }
//       while (node != (*it_print).first.first.second)
//       {
// // 	printf("searching %d ",(*it_print).second[node].next);
// 	it_print_path_2 = (*it_print).second.find((*it_print).second[node].next);
// 	printf("%d->",node);
// 	node = (*it_print_path_2).first;
// 	if (it_print_path_2 == (*it_print).second.end())
// 	  break;
//       }	
//       printf("%d\n",(*it_print).first.first.second);
//     }


}


void SPTsBroadcastLayer::computeAlternativePaths(std::list<Edge> explored_paths, ssim::ProcessId from, ssim::ProcessId to, unsigned int path_number)
{
//   printf("Computing alternative paths\n");

  //Remove this path from the graph and recompute Dijkstra (actually creat an auxiliary graph without this path)
  //printf ("Auxiliary graph\n");
  NTable N_aux(nodes.size());
  for(list<Edge>::const_iterator i = input.begin(); i != input.end(); ++i)
  {
    list<Edge>::iterator j = explored_paths.begin();
    bool inpath=false;
    while (j != explored_paths.end())
    {
      // 	      printf("comparing %d-%d  with %d-%d\n",(*j).from,(*j).to,(*i).from, (*i).to);
      if ( ( ( (*j).from == (*i).from ) && ( (*j).to == (*i).to ) ) || 
         ( ( (*j).from == (*i).to ) && ( (*j).to == (*i).from ) )     )
      {
	//link in the path do not add to aux path
	inpath=true;
	break;
      }
      j++;
    }
    if (inpath==false)
    {
      N_aux.add_edge((*i).from, (*i).to, (*i).len);
      //printf("inserting from: %d to: %d \n", (*i).from, (*i).to);
    }
	    //printf("Adding link from:%d to:%d length: %e\n",(*i).from,(*i).to,(*i).len);
  }
//   printf("Computing new disjoint path for %d-%d\n",from,to);
  list<Edge> new_path = dijkstra(N_aux,from,to);
  if (! new_path.empty())
  {
    //add this path to the explored path and recursively call computeAlternativePaths
//     printf("Another path exist\n");
    BOneWay T_ij;
    BOneWay T_ji;
    for (list<Edge>::iterator it = new_path.begin();it != new_path.end();it++)
    {
       //save the discovered path
//       printf("Saving link from:%d to:%d length: %e\n",(*it).from,(*it).to,(*it).len);
      BLinkOneWay & Path_ij = T_ij[(*it).to];
      BLinkOneWay & Path_ji = T_ji[(*it).from];
      //adding next node for the alternative path (in both directions i->j and j->i)
      Path_ij.next =(*it).from;
      Path_ji.next = (*it).to;
// 		T_ij.hop_count = hop_count;
// 		T_i[N[prev_ij].pid].next.insert(N[j].pid);
// 
// 		T_ji.prev = N[prev_ji].pid;
// 		T_ji.hop_count = hop_count;
// 		T_j[N[prev_ji].pid].next.insert(N[i].pid);
      explored_paths.push_back(Edge((*it).from,(*it).to,0));
    }
    alternative_paths.insert(std::make_pair(std::make_pair(std::make_pair(from,to),path_number),T_ij));
    alternative_paths.insert(std::make_pair(std::make_pair(std::make_pair(to,from),path_number),T_ji));
    path_number++;
    computeAlternativePaths(explored_paths,from,to,path_number);
  }
//   else
//   {
//     printf("Another path does not exist\n");
//   }
}

ProcessId SPTsBroadcastLayer::getNext(ProcessId from, ProcessId to, ProcessId current_node, unsigned int path_number) const {
    AlternativePaths::const_iterator pi = alternative_paths.find(std::make_pair(std::make_pair(from,to),path_number));
    if (pi == alternative_paths.end()) return NULL_PROCESSID;
    
    BOneWay::const_iterator bi = (*pi).second.find(current_node);
    if (bi == (*pi).second.end()) return NULL_PROCESSID;
    
    return (*bi).second.next;
}

const set<ProcessId>& SPTsBroadcastLayer::getChildren(ProcessId root, 
						      ProcessId node) const {
    static set<ProcessId> nullset;

    Paths::const_iterator pi = paths.find(root);
    if (pi == paths.end()) return nullset;
    
    B::const_iterator bi = (*pi).second.find(node);
    if (bi == (*pi).second.end()) return nullset;
    
    return (*bi).second.next;
}

ProcessId SPTsBroadcastLayer::getParent(ProcessId root, ProcessId node) const {
    Paths::const_iterator pi = paths.find(root);
    if (pi == paths.end()) return NULL_PROCESSID;
    
    B::const_iterator bi = (*pi).second.find(node);
    if (bi == (*pi).second.end()) return NULL_PROCESSID;
    
    return (*bi).second.prev;
}

int SPTsBroadcastLayer::getHopCount(ProcessId from, ProcessId to) const {
    Paths::const_iterator pi = paths.find(to);
    if (pi == paths.end()) return -1;
    
    B::const_iterator bi = (*pi).second.find(from);
    if (bi == (*pi).second.end()) return -1;
    
    return (*bi).second.hop_count;
}
