// -*- C++ -*-
//
//  This file is part of CBNSIM, the Content-Based Networking
//  Simulation Library.
//
//  Authors: See the file AUTHORS for full details. 
//
//  Copyright (C) 2009-2010 France Telecom R&D
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
//  along with this program
//
// $Id: Crandom.h,v 1.0 2009-10-20 14:30:30 feng Exp $
//
/*  This c++ file is done based on random.c	  
**  Module  : random.c
**  Version : 1.2
**  Date    : 25/03/97
*/	  

#include <stdio.h>
#include <math.h>
#include "cbnsim/Crandom.h"

using namespace cbnsim;

long Crandom :: rnd32(long seed)
{
 long times, rest, prod1, prod2;

 times = seed / LASTXN;
 rest  = seed - times * LASTXN;
 prod1 = times * UPTOMOD;
 prod2 = rest * A;
 seed  = prod1 + prod2;
 if (seed < 0) seed = seed + MODULE;
 return (seed);
}
  
double Crandom :: uniform(double a, double b, long *seed)
{
 double u;
 *seed = rnd32 (*seed);
 u = (*seed) * RATIO;
 u = a + u * (b-a);
 return (u);
}
	  
double Crandom :: uniform01(long *seed)
{
 double u;
 *seed = rnd32 (*seed);
 u = (*seed) * RATIO;
 return (u);
}
	  
double Crandom :: negexp (double mean, long *seed)
{
 double u;
 *seed = rnd32 (*seed);
 u = (*seed) * RATIO;
 return ( - mean*log(u) );
}

double Crandom :: pareto(double alpha , double kappa, long *seed)
{
 double u;
 *seed = rnd32 (*seed);
 u = (*seed) * RATIO;
 return ( kappa/pow(u,(1./alpha)) );
}


double Crandom :: pareto_m(double alpha , double mean, long *seed)
{
 double u;
 double kappa;
 if (alpha <= 1.0)
   { printf("Error pareto_m(): alpha must be greater than 1.0\n");
     return(mean);
   }
 *seed = rnd32 (*seed);
 u = (*seed) * RATIO;
 kappa = (alpha-1.0)*mean/alpha;
 return ( kappa/pow(u,(1./alpha)) );
}
	  
int Crandom :: poisson(double alpha,long *seed)
{
 int n = 0;
 double pn,lim;
 double prob;

 lim = pn = exp(-alpha);
 prob = uniform01(seed);
 while(prob>lim)
  { n++;
    pn *= alpha/n;
    lim += pn;
  }
 return(n);
}
	  
int Crandom :: geometric0(double mean,long *seed)
{
 int n;
 double prob,status;

 status = mean/(1.0+mean);	    /* E[X] = p/(1-p) -> p = E[X]/(1+E[X])  */
 prob = uniform01(seed);	    /* 1-p = prob. di avere n = 0 	    */
 n = (int)floor(log(1-prob)/log(status));
 return(n);
}
  
int Crandom :: geometric1(double mean,long *seed)
{
 int n;
 double prob,status;

 status = (mean-1)/mean;	    /* E[X] = 1/(1-p) -> p = (E[X]-1)/E[X]  */
 prob = uniform01(seed);	    /* 1-p = prob. di avere n = 1 	    */
 n = 1 + (int)floor(log(1-prob)/log(status));
 return(n);
}
	  
int Crandom :: geometric_trunc1(double mean,int max_len,long *seed)
{
 int n;
 double prob,f_p,df_p,
        temp_status,temp_res,len;
 static double status   = 0.0,
               old_mean = 0.0,
               status_N = 0.0;
 static int    old_max  = 0;
 
 if (mean>=(double)max_len)
   { printf("Error Calling Geometric_Trunc1()\n");
     return(1);
   }
 if ( fabs(old_mean-mean)>1e-5 || old_max!=max_len)
  {
    len = (double)max_len;
    temp_status = (mean-1)/mean;
    do
     {
      status = temp_status;
      status_N = pow(status,len);
      temp_res = (mean-len) * status + len-mean + 1;
      f_p = status_N * temp_res - mean * status + mean - 1;
      df_p = (mean-len) * status_N + len * status_N * temp_res / status - mean;
      temp_status = status - f_p / df_p;
     }
    while (fabs(temp_status-status)>1e-9);
    status = temp_status;
    status_N = pow(status,len);
    old_mean = mean;
    old_max = max_len;
  } 

 prob = uniform01(seed);
 n = 1 + (int)floor(log(1 - prob + prob * status_N)/log(status));
 return(n);
}
	  
int Crandom :: trunc_exp(double mean,long length,long *seed)
{
 double len,prob;

 *seed = rnd32 (*seed);
 prob = (*seed) * RATIO;
 /* len =  - 8*mean*(log(*seed)-21.4875626); */
 len = -8*mean*log(prob);
 len = (len>length*8.) ? length : len/8.;
 return( (int)len == 0 ? 1 : (int)len );
}

