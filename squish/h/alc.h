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

/*# name=Include file to pick between MALLOC.H and ALLOC.H
*/

#ifndef __ALC_H_DEFINED
#define __ALC_H_DEFINED

#include "compiler.h"

#if defined(_lint) || defined(__MSC__) || defined(__WATCOMC__) || defined(__IBMC__) || defined(__TOPAZ__)
  #include <malloc.h>

  #ifdef __FARDATA__

  /* for some insane reason the turbo-c coreleft() function changes
   * it's return value based on the memory model.
   */

    unsigned long cdecl coreleft   (void);
  #else
    unsigned cdecl coreleft        (void);
  #endif

#elif defined(__TURBOC__)
  #include <alloc.h>
#else
  #include <string.h>
#endif

#ifdef __TURBOC__
#define halloc(x,y) ((char far *)farmalloc(x*y))
#define hfree(p)    farfree(p)
#endif

#endif /* __ALC_H_DEFINED */

