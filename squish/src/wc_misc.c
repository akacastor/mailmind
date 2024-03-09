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

/*# name=Replacement functions for WATCOM C
*/

// This should only be used for WC!

#ifdef __WATCOMC__

#include <mem.h>
#include <dos.h>
#include <conio.h>
#include <errno.h>
#include "prog.h"
#include "alc.h"

int _fast wc_grow_handles(int n)
{
  return _grow_handles(n > 101 ? n : 101);
}

#if defined(__MSDOS__)
  int _stdc getcurdir(int drive, char *directory)
  {
    union REGS r;

    r.h.ah=0x47;
    r.h.dl=(byte)drive;

  #ifdef __FLAT__
    r.x.esi=FP_OFF(directory);
    int386(0x21, &r, &r);
  #else
    {
      struct SREGS sr;

      sr.ds= FP_SEG(directory);
      r.x.si=FP_OFF(directory);
      int86x(0x21, &r, &r, &sr);
    }
  #endif

    return (r.x.cflag==0 ? 0 : -1);
  }
#elif defined(OS_2)

  #define INCL_DOS
  #include <os2.h>

  int _stdc getcurdir(int drive, char *directory)
  {
  #ifdef __FLAT__
    ULONG buflen=PATHLEN;

    return (DosQueryCurrentDir((ULONG)drive, directory, &buflen)==0 ? 0 : -1);
  #else
    USHORT buflen=120;

    return (DosQCurDir(drive, directory, &buflen)==0 ? 0 : -1);
  #endif
  }

#elif defined(NT)

  #include "pwin.h"

  int _stdc getcurdir(int drive, char *directory)
  {
    ULONG buflen=PATHLEN;
    int old_disk, rc;

    old_disk=getdisk();

    setdisk(drive);
    rc=GetCurrentDirectory(PATHLEN, directory);
    setdisk(old_disk);

    return (rc > 0 ? 0 : -1);
  }

#endif


#ifndef __FLAT__
  void far *farcalloc(int n,int m)
  {
    void far *p;

    p=malloc(n*m);

    if (!p)
      return NULL;

  #ifdef __FLAT__
    memset(p,'\0',m*n);
  #else
    _fmemset(p,'\0',m*n);
  #endif

    return p;
  }
#endif /* !__FLAT__ */

#endif /* __WATCOMC__ */
