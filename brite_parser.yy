/* -*- C++ -*- */
%{
#include <cstdio>
#include <map>
#include <set>
#include <string>
#include <list> 

#define YY_DECL int yylex()

  extern YY_DECL;
  
  int yyerror(const char* s);
  
  using namespace std;
  
  map<int, set<int> > topo_adjacency_list;
  map< pair<int,int>, list<double> > topo_link_lengths;
  map<int, pair<int,int> > topo_node_coords;
  map<int, int > topo_node_type;
  map<int, pair<int,char> > topo_node_cache;
%}

%union
{
  int ival;
  double dval;
  char cval;
}

%token TOPOLOGY_LINE MODEL_LINE NODES_LINE EDGES_LINE
%token TOK_AS_NODE TOK_RT_NODE TOK_E_AS TOK_E_RT TOK_U

%token <ival> INT
%token <dval> DOUBLE
%token <cval> CHAR

%%

text: topology
;

topology: TOPOLOGY_LINE MODEL_LINE nodes_section edges_section
;

nodes_section: NODES_LINE  node_list
;

node_list: node_line | node_list node_line
;

node_line: INT INT INT INT CHAR INT node_type
{
  // $1 = id
  // $2 = x-coord
  // $3 = y-coord
  // $4 = indegree
  // $5 = outdegree
  // $6 = asid
  topo_adjacency_list[$1] = set<int>();
  topo_node_coords[$1] = make_pair( $2, $3 );
  topo_node_cache[$1] = make_pair( $4, $5 );
  topo_node_type[$1] = $6;
}
;

node_type: TOK_AS_NODE | TOK_RT_NODE
;

edges_section: EDGES_LINE edge_list
;

edge_list: edge_line | edge_list edge_line
;

edge_line: INT INT INT DOUBLE DOUBLE DOUBLE INT INT edge_type TOK_U
{
  // $1 = id
  // $2 = from
  // $3 = to
  // $4 = length *It was originally destined to length of the link, modify the 4th field of the brite topology, it will represent the size of the buffer for this link
  // $5 = delay *proportional to the placement of the nodes (with brite generate random placed nodes)
  // $6 = bandwidth
  // $7 = ASfrom
  // $8 = ASto
  // $9 = type
  // $10 = ??
  topo_adjacency_list[$2].insert( $3 );
  topo_link_lengths[make_pair( $2, $3 )].push_back( $5 );
  topo_link_lengths[make_pair( $2, $3 )].push_back( $4 );
  topo_link_lengths[make_pair( $2, $3 )].push_back( $6 );
}
;

edge_type: TOK_E_AS | TOK_E_RT
;

%%

int yyerror( const char * s )
{
  fprintf( stderr, s );
  return 0;
}
