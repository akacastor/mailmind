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

#include <io.h>
#include <dos.h>
#include "prog.h"

#if defined(OS_2)

  #define INCL_NOPM
  #define INCL_DOS
  #include "pos2.h"

  void pascal far flush_handle2( int fh )
  {
  #ifdef __FLAT__
    DosResetBuffer((HFILE)fh);
  #else
    DosBufReset((HFILE)fh);
  #endif
  }

#elif defined(NT)
  #include "pwin.h"

  void pascal far flush_handle2( int fh )
  {
    FlushFileBuffers((HANDLE)fh);
  }

#elif !defined(__MSDOS__)  /* dos version defined in .asm file */
  #error Unknown OS
#endif


/* This makes sure that a file gets flushed to disk. */

void _fast flush_handle(FILE *fp)
{

  fflush(fp);

#if defined(OS_2) || defined(NT) || defined(MSDOS) || defined(__MSDOS__) || defined(__TURBOC__)
  flush_handle2(fileno(fp));
#else
  {
    int nfd;

    if ((nfd=dup(fileno(fp))) != -1)
      close(nfd);
  }
#endif
}


