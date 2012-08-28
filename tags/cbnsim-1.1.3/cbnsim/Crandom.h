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
/*  This head file is done based on random.c	  
**  Module  : random.c
**  Version : 1.2
**  Date    : 25/03/97
*/	  

#define MODULE  2147483647
#define A       16807
#define LASTXN  127773
#define UPTOMOD -2836
#define RATIO   0.46566128e-9            /* 1/MODULE */

#include <stdio.h>
#include <math.h>

namespace cbnsim
{
  /** @brief generation of different distribution samples.
   */
  class Crandom
  {
  public:
    /*	  
    **  Function: long rnd32(long seed) 
    **  Remarks : Congruential generator of pseudorandom sequences of numbers
    **            uniformly distributed between 1 and 2147483646, using the con-
    **            gruential relation: Xn+1 = 16807 * Xn  mod  2147483647 . 
    **            The input to the routine is the integer Xn, while the returned 
    **            integer is the number Xn+1.
    */
    long rnd32(long seed);

    /*
    **  Function: double uniform(double a, double b, long *seed)
    **  Return  : a value uniformly distributed between 'a' and 'b'
    **  Remarks : The value of '*seed' is changed
    */	  
    double uniform(double a, double b, long *seed);

    /*
    **  Function: double uniform01(long *seed)
    **  Return  : a value uniformly distributed between 0.0 and 1.0
    **  Remarks : The value of '*seed' is changed
    */	  
    double uniform01(long *seed);

    /*	  
    **  Function: double negexp(double mean, long *seed)
    **  Return  : a value exponentially distributed with mean 'mean'
    **  Remarks : The value of '*seed' is changed
    */	  
    double negexp (double mean, long *seed);

    /*
    **  Function: double pareto(double alpha , double kappa, long *seed)
    **  Return  : a value pareto distributed with paremeters k and alpha
    **  Remarks : The value of '*seed' is changed
    */
    /*  -MMM- Qui forse occorre passare come parametri alpha e il valor 
        medio... 
    */
    double pareto(double alpha , double kappa, long *seed);

    /*
    **  Function: double pareto_m(double alpha , double kappa, long *seed)
    **  Return  : a value pareto distributed with paremeter alpha and mean 'mean'
    **  Remarks : The value of '*seed' is changed. Valid only if alpha > 1.0
    */
    double pareto_m(double alpha , double mean, long *seed);

    /*	  
    **  Function: int poisson(double alpha,long *seed)
    **  Return  : The number of users arrived, according to a poisson process 
    **            with rate 'alpha' user/slot, in a slot
    **  Remarks : The value of '*seed' is changed
    */	  
    int poisson(double alpha,long *seed);

    /*	  
    **  Function: int geometric0(double mean,long *seed)
    **  Return  : a random value distributed geometrically with average 'mean',
    **            starting from 0. (0 w.p. 1-p, 1 w.p. p(1-p), etc.) 
    **  Remarks : The value of '*seed' is changed
    */	  
    int geometric0(double mean,long *seed);

    /*	  
    **  Function: int geometric1(double mean,long *seed)
    **  Return  : a random value distributed geometrically with average 'mean',
    **            starting from 1. (1 w.p. 1-p, 2 w.p. p(1-p), etc.) 
    **  Remarks : The value of '*seed' is changed
    */	  
    int geometric1(double mean,long *seed);

    /*	  
    **  Function: int geometric_trunc1(double mean,int max_len,long *seed)
    **  Return  : a random value distributed geometrically with average 'mean',
    **            starting from 1.
    **            The distribution is truncated at the value 'max_len'.
    **  Remarks : The value of '*seed' is changed
    */	  
    int geometric_trunc1(double mean,int max_len,long *seed);
  /* These function returns a number distributed quasi-geometrically with   */
  /* average mean and maximum value 'max_len'.				    */
  /* There are some problems with the calculation. Here we explain the way  */
  /* the numbers are calculated.					    */
  /* The mean value of the random variable is:				    */
  /*                                                                        */
  /*			 Sum(i*p^(i-1),i=1..N)				    */
  /*		  E[x] = --------------------- = m			    */
  /*			  Sum(p^(i-1),i=1..N)				    */
  /* i.e.								    */
  /*		         p^N ( Np - N - 1) + 1				    */
  /*		     m = ---------------------		(1)		    */
  /*			     (1-p)(1-p^N)				    */
  /*    								    */
  /* where p is the transition probability in the Markov chain of the model.*/
  /*                                                                        */
  /* We need the value of p as a function of m and N. The only solution is  */
  /* to solve the equation (1) in the variable p using the Newton method,   */
  /* i.e.                                                                   */
  /*           p' = p - f(p)/f'(p)                                          */
  /* being p' the value of p at the step i+1, p the value at the step i,    */
  /* f(p) is (1) and f'(p) is df(p)/dp.                                     */
  /* In our calculations, we use:                                           */
  /*                                                                        */
  /*    f(p)  = p^N * ((m-N)p + N - m + 1) - mp + m -1                      */
  /*    f'(p) = (m-N) p^N + N p^(N-1)((m-N)p + N - m + 1) - m               */
  /*                                                                        */
  /* and the value  p = (m-1)/m  as starting point. This is the value of    */
  /* p when N tends to infinity.					    */
  /*                                                                        */
  /* This value of p is used to find the number n to be returned.  A random */
  /* variable q uniformly distributed in (0,1) is extracted, so if	    */
  /*                                                                        */
  /*               sum(p^(i-1),i=1..n)    1 - p^n                           */
  /*          q = --------------------- = -------                           */
  /*               sum(p^(i-1),i=1..N)    1 - p^N                           */
  /*                                                                        */
  /* we found that                                                          */
  /*                                                                        */
  /*		  |~  log(p^N * q - q - 1) ~|				    */
  /*	      n = |   --------------------  |				    */
  /*		  |         log(p)	    |				    */
  /*                                                                        */
  /* In order to avoid large computations, the previous values of 'mean'    */
  /* and 'max_len' are recorded, so if the function is called twice or more */
  /* times consecutively with the same parameters, the previously computed  */
  /* value of p can be used.						    */
  /*                                                                        */
  /* In the code, there is the corrispondence:                              */
  /*		p     -> status						    */
  /*		m     -> mean						    */
  /*		N     -> max_len					    */
  /*		q     -> prob						    */
  /*		f(p)  -> f_p						    */
  /*		f'(p) -> df_p						    */
  /* between the symbols used in this comment and the variables names.      */



    /*	  
    **  Function: int trunc_exp(double mean,long length,long *seed)
    **  Return  : mean and length are in bytes;
    **  Remarks : The value of '*seed' is changed
    */	  
    int trunc_exp(double mean,long length,long *seed);
  };
};

