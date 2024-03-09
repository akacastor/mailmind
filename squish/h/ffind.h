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

/*# name=FFIND.C include file.
    credit=Thanks to Peter Fitzsimmons for this module.
*/

#ifndef __FFIND_H_DEFINED
#define __FFIND_H_DEFINED

#include "compiler.h"
#include "typedefs.h"
#include "stamp.h"
#include "prog.h"

struct _ffind;
typedef struct _ffind FFIND;

#if defined(__MSDOS__)

  /* Layout of the DOS Disk Transfer Area */

  struct _dosdta
  {
    char achReserved[21];
    byte bAttr;
    word usTime;
    word usDate;
    dword ulSize;
    char achName[13];
  };

  unsigned far pascal __dfindfirst(char far *name, unsigned attr, struct _dosdta far *dta);
  unsigned far pascal __dfindnext(struct _dosdta far *dta);

#elif defined(NT)
  #include "pwin.h"
#endif


struct _ffind
{
  word usAttr;
  SCOMBO scCdate;
  SCOMBO scAdate;
  SCOMBO scWdate;
  dword ulSize;

#if defined(__MSDOS__)

  char szName[13];
  struct _dosdta __dta;

#elif defined(NT)

  char szName[PATHLEN];
  HANDLE hdir;

#elif defined(OS_2)

  char szName[PATHLEN];

  #ifdef __FLAT__ /* OS/2 2.0 or NT */
    unsigned long hdir;
  #else
    unsigned short hdir;       /* directory handle from DosFindFirst */
  #endif

#endif
};

FFIND * _fast FindOpen(char *filespec,unsigned short attribute);
FFIND * _fast FindInfo(char *filespec); /*PLF Thu  10-17-1991  18:03:09 */
int _fast FindNext(FFIND *ff);

#ifdef NT /* NT's abortion of a naming convention conflicts with our usage! */
#define FindClose FndClose
#endif

void _fast FindClose(FFIND *ff);

#define ATTR_READONLY  0x01
#define ATTR_HIDDEN    0x02
#define ATTR_SYSTEM    0x04
#define ATTR_VOLUME    0x08
#define ATTR_SUBDIR    0x10
#define ATTR_ARCHIVE   0x20
#define ATTR_RSVD1     0x40
#define ATTR_RSVD2     0x80

#define MSDOS_READONLY  ATTR_READONLY
#define MSDOS_HIDDEN    ATTR_HIDDEN
#define MSDOS_SYSTEM    ATTR_SYSTEM
#define MSDOS_VOLUME    ATTR_VOLUME
#define MSDOS_SUBDIR    ATTR_SUBDIR
#define MSDOS_ARCHIVE   ATTR_ARCHIVE
#define MSDOS_RSVD1     ATTR_RSVD1
#define MSDOS_RSVD2     ATTR_RSVD2

#endif /* __FFIND_H_DEFINED */

