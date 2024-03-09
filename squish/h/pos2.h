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

#ifndef __POS2_H_DEFINED
#define __POS2_H_DEFINED

#ifdef __FLAT__
/*#pragma pack(4)*/
#pragma pack(1)
#else
#pragma pack(1)
#endif

#ifdef __FLAT__
  #define OS2UINT  ULONG
  #define DosTransactNmPipe DosTransactNPipe
  #define DosPeekNmPipe DosPeekNPipe
  #define DosSetNmPHandState DosSetNPHState

  #ifdef NULL
    #undef NULL
  #endif
#else
  #define OS2UINT  USHORT
  #define APIENTRY16 APIENTRY
#endif

#include "pm32.h"
#include <os2.h>
#pragma pack()

#undef ADDRESS

#ifdef __WATCOMC__
  #ifdef __FLAT__
    #pragma library("os2386.lib");
  #else
    #pragma library("os2.lib");
  #endif
#endif

#endif /* __POS2_H_DEFINED */

