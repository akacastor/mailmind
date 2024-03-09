/***************************************************************************
 *                                                                         *
 *  Squish Developers Kit Source, Version 2.00                             *
 *  Copyright 1989-1994 by SCI Communications.  All rights reserved.       *
 *                                                                         *
 *  USE OF THIS FILE IS SUBJECT TO THE RESTRICTIONS CONTAINED IN THE       *
 *  SQUISH DEVELOPERS KIT LICENSING AGREEMENT IN SQDEV.PRN.  IF YOU DO NOT *
 *  FIND THE TEXT OF THIS AGREEMENT IN THE AFOREMENTIONED FILE, OR IF YOU  *
 *  DO NOT HAVE THIS FILE, YOU SHOULD IMMEDIATELY CONTACT THE AUTHOR AT    *
 *  ONE OF THE ADDRESSES LISTED BELOW.  IN NO EVENT SHOULD YOU PROCEED TO  *
 *  USE THIS FILE WITHOUT HAVING ACCEPTED THE TERMS OF THE SQUISH          *
 *  DEVELOPERS KIT LICENSING AGREEMENT, OR SUCH OTHER AGREEMENT AS YOU ARE *
 *  ABLE TO REACH WITH THE AUTHOR.                                         *
 *                                                                         *
 *  You can contact the author at one of the address listed below:         *
 *                                                                         *
 *  Scott Dudley       FidoNet     1:249/106                               *
 *  777 Downing St.    Internet    sjd@f106.n249.z1.fidonet.org            *
 *  Kingston, Ont.     CompuServe  >INTERNET:sjd@f106.n249.z1.fidonet.org  *
 *  Canada  K7M 5N3    BBS         1-613-634-3058, V.32bis                 *
 *                                                                         *
 ***************************************************************************/

/*# name=Integer quicksort routine.
    credit=Thanks to Thomas Plum and _Reliable_Data_Structure_In_C_
    credit=for this routine.
*/

#include <stdio.h>
#include "prog.h"

#define NUM sizeof(array)/sizeof(array[0])
#define SWAP(a,b,s) s=a; a=b; b=s;

static void _fast iqksort(int *p_lo,int *p_hi);

void _fast qksort(int a[],size_t n)
{
  if (n > 1)
    iqksort(a,&a[n-1]);
}



static void _fast iqksort(int *p_lo,int *p_hi)
{
  int  *p_mid=p_lo+(((int)(p_hi-p_lo))/2),
       *p_i,
       *p_lastlo,
       tmp;

  SWAP(*p_lo,*p_mid,tmp);

  p_lastlo=p_lo;

  for (p_i=p_lo+1;p_i <= p_hi;++p_i)
  {
    if (*p_lo > *p_i)
    {
      ++p_lastlo;
      SWAP(*p_lastlo,*p_i,tmp);
    }
  }

  SWAP(*p_lo,*p_lastlo,tmp);

  if (p_lo < p_lastlo && p_lo < p_lastlo-1)
    iqksort(p_lo,p_lastlo-1);

  if (p_lastlo+1 < p_hi)
    iqksort(p_lastlo+1,p_hi);
}

