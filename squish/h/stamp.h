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

#ifndef __STAMP_H_DEFINED
#define __STAMP_H_DEFINED

#include "typedefs.h"

struct _stamp   /* DOS-style datestamp */
{
  struct
  {
    /* IBM Cset/2 is allergic to "unsigned short" when declaring bitfields! */

#ifdef __IBMC__
    unsigned int da : 5;
    unsigned int mo : 4;
    unsigned int yr : 7;
#else
/*lint -e46 */
    word da : 5;
    word mo : 4;
    word yr : 7;
/*lint -restore */
#endif
  } date;

  struct
  {
#ifdef __IBMC__
    unsigned int ss : 5;
    unsigned int mm : 6;
    unsigned int hh : 5;
#else
/*lint -e46 */
    word ss : 5;
    word mm : 6;
    word hh : 5;
/*lint -restore */
#endif
  } time;
};


struct _dos_st
{
  word date;
  word time;
};

/* Union so we can access stamp as "int" or by individual components */

union stamp_combo   
{
  dword ldate;
  struct _stamp msg_st;
  struct _dos_st dos_st;
};

typedef union stamp_combo SCOMBO;

#endif /* __STAMP_H_DEFINED */

