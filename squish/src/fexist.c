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

/*# name=File-exist and directory-searching routines
*/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#if (defined(__MSDOS__) || defined(OS_2) || defined(NT)) && !defined(__IBMC__)
#include <dos.h>
#endif

#include "ffind.h"
#include "prog.h"


/*
main()
{
  printf("asdfe=%d\n",direxist("c:\\asdfe"));
  printf("blank=%d\n",direxist("c:\\blank"));
  printf("tc=%d\n",direxist("c:\\tc"));
  printf("c:\\=%d\n",direxist("c:\\"));
  printf("d:\\=%d\n",direxist("d:\\"));
  printf("e:\\=%d\n",direxist("e:\\"));
  printf("f:\\=%d\n",direxist("f:\\"));
}
*/


int _fast fexist(char *filename)
{
  FFIND *ff;

  ff=FindOpen(filename,0);

  if (ff)
  {
    FindClose(ff);
    return TRUE;
  }
  else return FALSE;
}

long _fast fsize(char *filename)
{
  FFIND *ff;
  long ret=-1L;

  ff=FindOpen(filename, 0);

  if (ff)
  {
    ret=ff->ulSize;
    FindClose(ff);
  }

  return ret;
}

#if defined(__MSDOS__)
  int _fast direxist(char *directory)
  {
    FFIND *ff;
    char *tempstr;
    int ret;

    if ((tempstr=(char *)malloc(strlen(directory)+5))==NULL)
      return FALSE;

    strcpy(tempstr, directory);

    Add_Trailing(tempstr,'\\');

    /* Root directory of any drive always exists! */

    if ((isalpha(tempstr[0]) && tempstr[1]==':' &&
        ((tempstr[2]=='\0') ||
         (tempstr[2]=='\\' || tempstr[2]=='/') && tempstr[3]=='\0')) ||
        eqstri(tempstr, "\\"))
    {
      ret=TRUE;
    }
    else
    {
      Strip_Trailing(tempstr, '\\');

      ff=FindOpen(tempstr, ATTR_SUBDIR | ATTR_HIDDEN | ATTR_READONLY);

      ret=(ff != NULL && (ff->usAttr & ATTR_SUBDIR));

      if (ff)
        FindClose(ff);
    }

    free(tempstr);
    return ret;

  }
#else
  #include "uni.h"

  int _fast direxist(char *directory)
  {
    int ret;
    char *tempstr;
    size_t l;

    if (NULL == (tempstr=(char *)strdup(directory)))
      return FALSE;

    /* Root directory of any drive always exists! */

#ifdef UNIX
    if (eqstr(tempstr, "/"))
#else
    if ((isalpha(tempstr[0]) && tempstr[1]==':' &&
        (tempstr[2]=='\\' || tempstr[2]=='/') && !tempstr[3]) ||
        eqstr(tempstr, "\\"))
#endif
    {
      free(tempstr);
      return TRUE;
    }

    l = strlen(tempstr);
    if( tempstr[l-1] == '\\' || tempstr[l-1] == '/')
      tempstr[l-1] = 0;           /* remove trailing backslash */

    ret = !access(tempstr, 0);

    free(tempstr);
    return ret;
  }
#endif

