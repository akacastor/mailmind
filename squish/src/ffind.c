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

/*# name=Portable file-searching hooks
    credit=Thanks go to Peter Fitzsimmons for these routines.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

#ifndef __IBMC__
#include <dos.h>
#endif

#ifdef __TURBOC__
#include <dir.h>
#endif

#include "ffind.h"

#if defined(OS_2)
  #define INCL_NOPM
  #define INCL_DOS
  #include <os2.h>

  #ifdef __FLAT__
    static void near CopyFBUF2FF(FFIND *ff, FILEFINDBUF3 *findbuf)
  #else
    static void near CopyFBUF2FF(FFIND *ff, FILEFINDBUF *findbuf)
  #endif
  {
    ff->usAttr=findbuf->attrFile;
    ff->ulSize=findbuf->cbFile;

    ff->scWdate.dos_st.time=*((USHORT *)&findbuf->ftimeLastWrite);
    ff->scWdate.dos_st.date=*((USHORT *)&findbuf->fdateLastWrite);

    ff->scCdate.dos_st.time=*((USHORT *)&findbuf->ftimeCreation);
    ff->scCdate.dos_st.date=*((USHORT *)&findbuf->fdateCreation);

    ff->scAdate.dos_st.time=*((USHORT *)&findbuf->ftimeLastAccess);
    ff->scAdate.dos_st.date=*((USHORT *)&findbuf->fdateLastAccess);

    if (!ff->scCdate.ldate)
      ff->scCdate.ldate=ff->scWdate.ldate;

    if (!ff->scAdate.ldate)
      ff->scAdate.ldate=ff->scWdate.ldate;

    strncpy(ff->szName, findbuf->achName, sizeof(ff->szName));
  }

  /* Find first file in list */

  FFIND * _fast FindOpen(char *filespec, unsigned short attribute)
  {
    FFIND *ff;

    ff=malloc(sizeof(FFIND));

    if (ff)
    {
      #ifdef __FLAT__ /* OS/2 2.0 */
        ULONG usSearchCount=1;
        FILEFINDBUF3 findbuf;
      #else
        USHORT usSearchCount = 1;
        FILEFINDBUF findbuf;
      #endif


      ff->hdir=HDIR_CREATE;

      #ifdef __FLAT__
        if (DosFindFirst(filespec, &ff->hdir, attribute, &findbuf,
                         sizeof findbuf, &usSearchCount, FIL_STANDARD)==0)
      #else
        if (DosFindFirst(filespec, &ff->hdir, attribute, &findbuf,
                         sizeof findbuf, &usSearchCount, 0L)==0)
      #endif
        {
          CopyFBUF2FF(ff, &findbuf);
        }
        else
        {
          free(ff);
          ff=NULL;
        }
    }

    return ff;
  }

  int _fast FindNext(FFIND *ff)
  {
    int rc=-1;

    if (ff)
    {
      #ifdef __FLAT__
        ULONG usSearchCount=1;
        FILEFINDBUF3 findbuf;
      #else
        USHORT usSearchCount=1;
        FILEFINDBUF findbuf;
      #endif

      if (ff->hdir &&
          DosFindNext(ff->hdir, &findbuf, sizeof findbuf, &usSearchCount)==0)
      {
        CopyFBUF2FF(ff, &findbuf);
        rc=0;
      }
    }

    return rc;
  }

  void _fast FindClose(FFIND *ff)
  {
    if (ff)
    {
      if (ff->hdir)
        DosFindClose(ff->hdir);

      free(ff);
    }
  }

  /* This function was added because it is SIGNIFICANTLY faster under OS/2 to
   * call DosQPathInfo() rather than DosFindFirst(),   if all you are
   * intested in is getting a specific file's date/time/size.
   *
   *PLF Thu  10-17-1991  18:12:37
   */

  FFIND * _fast FindInfo(char *filespec)
  {
    FFIND *ff;
    FILESTATUS fs;
    char *f;

    ff = malloc(sizeof(*ff));
    if(!ff)
        return NULL;

    memset(ff, 0, sizeof(*ff));

  #ifdef __FLAT__
    if (DosQueryPathInfo(filespec, FIL_STANDARD, (PBYTE)&fs, sizeof(fs))==0)
  #else
    if (DosQPathInfo(filespec, FIL_STANDARD, (PBYTE)&fs, sizeof(fs), 0L)==0)
  #endif
    {
      ff->usAttr=fs.attrFile;
      ff->ulSize=fs.cbFile;

      ff->scWdate.dos_st.time=*((USHORT *)&fs.ftimeLastWrite);
      ff->scWdate.dos_st.date=*((USHORT *)&fs.fdateLastWrite);

      ff->scCdate.dos_st.time=*((USHORT *)&fs.ftimeCreation);
      ff->scCdate.dos_st.date=*((USHORT *)&fs.fdateCreation);

      ff->scAdate.dos_st.time=*((USHORT *)&fs.ftimeLastAccess);
      ff->scAdate.dos_st.date=*((USHORT *)&fs.fdateLastAccess);



      if ((f=strrchr(filespec, '\\'))==NULL)
        f=filespec;
      else f++;

      strncpy(ff->szName, f, sizeof(ff->szName));
    }
    else
    {
      free(ff);
      return NULL;
    }

    return ff;
  }

#elif defined(NT)

  static void near CopyWFD2FF(FFIND *ff, WIN32_FIND_DATA *pwfd)
  {
    ff->usAttr=pwfd->dwFileAttributes;
    ff->ulSize=pwfd->nFileSizeLow;

    FileTimeToDosDateTime(&pwfd->ftLastWriteTime,
                          &ff->scWdate.dos_st.date,
                          &ff->scWdate.dos_st.time);

    FileTimeToDosDateTime(&pwfd->ftCreationTime,
                          &ff->scCdate.dos_st.date,
                          &ff->scCdate.dos_st.time);

    FileTimeToDosDateTime(&pwfd->ftLastAccessTime,
                          &ff->scAdate.dos_st.date,
                          &ff->scAdate.dos_st.time);

    strncpy(ff->szName, pwfd->cFileName, sizeof(ff->szName));
  }

  FFIND * _fast FindOpen(char *filespec, unsigned short attribute)
  {
    FFIND *ff;

    NW(attribute);

    ff=malloc(sizeof(FFIND));

    if (ff)
    {
      WIN32_FIND_DATA wfd;

      if ((ff->hdir=FindFirstFile(filespec, &wfd)) != INVALID_HANDLE_VALUE)
        CopyWFD2FF(ff, &wfd);
      else
      {
        free(ff);
        ff=NULL;
      }
    }

    return ff;
  }

  int _fast FindNext(FFIND *ff)
  {
    int rc=-1;

    if (ff)
    {
      WIN32_FIND_DATA wfd;

      if (ff->hdir != INVALID_HANDLE_VALUE && FindNextFile(ff->hdir, &wfd))
      {
        CopyWFD2FF(ff, &wfd);
        rc=0;
      }
    }

    return rc;
  }

  void _fast FndClose(FFIND *ff)
  {
    if (ff)
    {
      #undef FindClose

      if (ff->hdir != INVALID_HANDLE_VALUE)
        FindClose(ff->hdir);

      free(ff);
    }
  }

  FFIND * _fast FindInfo(char *filespec)
  {
    return FindOpen(filespec, 0);
  }

#elif defined(__MSDOS__)

  static void near CopyDTA2FF(FFIND *ff)
  {
    ff->usAttr=(word)ff->__dta.bAttr;

    ff->scCdate.dos_st.time=ff->__dta.usTime;
    ff->scCdate.dos_st.date=ff->__dta.usDate;

    /* Copy dates into other structure too */

    ff->scAdate=ff->scWdate=ff->scCdate;
    ff->ulSize=ff->__dta.ulSize;

    memset(ff->szName, '\0', sizeof(ff->szName));
    memmove(ff->szName, ff->__dta.achName, sizeof(ff->szName));
    strupr(ff->szName);
  }

  FFIND * _fast FindOpen(char *filespec, unsigned short attribute)
  {
    FFIND *ff;

    ff=malloc(sizeof(FFIND));

    if (ff)
    {
      if (__dfindfirst(filespec, attribute, &ff->__dta)==0)
        CopyDTA2FF(ff);
      else
      {
        free(ff);
        ff=NULL;
      }
    }

    return ff;
  }


  int _fast FindNext(FFIND *ff)
  {
    int rc=-1;

    if (ff)
    {
      if ((rc=__dfindnext(&ff->__dta))==0)
        CopyDTA2FF(ff);
    }

    return rc;
  }

  void _fast FindClose(FFIND *ff)
  {
    if (ff)
      free(ff);
  }

  FFIND * _fast FindInfo(char *filespec)
  {
    return FindOpen(filespec, 0);
  }


#else
  #error Unknown OS
#endif




#ifdef TEST_SHELL

#define TRUE 1
#define FALSE 0

int walk(char *path);

void main(int argc, char **argv)
{
    walk("\\");     /* start at root*/
}

/* this simple function assumes the path ALWAYS has an ending backslash */
walk(char *path)
{
    FFIND *ff;
    int done = FALSE;
    char full[66];

    strcpy(full, path);
    strcat(full, "*");
    if( ff = FindOpen(full, ATTR_SUBDIR) ){

        for(done = FALSE; !done; done = FindNext(ff)){
            if( (ff->usAttr & ATTR_SUBDIR) && (ff->szName[0] != '.') ){
                strcpy(full, path);
                strcat(full, ff->szName);
                puts(full);

                {
                  char temp[120];
                  FFIND *f;

                  strcpy(temp, full);
                  strcat(temp, "\\*");

                  if ((f=FindOpen(temp, 0)) != NULL)
                  {
                    do
                    {
                      printf("\t%s\n", f->szName);
                    }
                    while (FindNext(f)==0);

                    FindClose(f);
                  }

                }

                strcat(full, "\\");
                if( !walk(full) )
                    return(FALSE);
            }
        }
        FindClose(ff);
        return(TRUE);
    }
    else{
        puts("FindOpen() failed");
    }
    return(FALSE);
}

#endif

