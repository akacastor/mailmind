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

#ifndef __TYPEDEFS_H_DEFINED
#define __TYPEDEFS_H_DEFINED

/* Handle the special defines for 32-bit flat model compilers */

#if defined(__386__) || defined(__FLAT__)
  typedef unsigned      bit;

  typedef unsigned char byte;
  typedef signed char   sbyte;

  typedef unsigned short word;
  typedef signed short   sword;

  typedef unsigned int  dword;
  typedef signed int    sdword;

#if !defined(NT) && !defined(UNIX)
  typedef unsigned short ushort;
#endif

  typedef   signed short sshort;

  typedef unsigned long  ulong;
  typedef   signed long  slong;
#else
  typedef unsigned      bit;

  typedef unsigned char byte;
  typedef signed char   sbyte;

  typedef unsigned int   word;
  typedef signed int    sword;

  typedef unsigned long dword;
  typedef signed long   sdword;

  typedef unsigned short ushort;
  typedef   signed short sshort;

  typedef unsigned long  ulong;
  typedef   signed long  slong;
#endif

#endif

