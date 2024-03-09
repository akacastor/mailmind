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

#pragma off(unreferenced)
static char rcs_id[]="$Id: SQ_LOCK.C 1.3 1994/04/01 11:54:45 sjd Exp sjd $";
#pragma on(unreferenced)

#define MSGAPI_HANDLERS
#define MSGAPI_NO_OLD_TYPES

#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "prog.h"
#include "msgapi.h"
#include "api_sq.h"
#include "apidebug.h"


/* Lock the first byte of the Squish file header.  Do this up to            *
 * SQUISH_LOCK_RETRY number of times, in case someone else is using         *
 * the message base.                                                        */

static unsigned near _SquishLockBase(HAREA ha)
{
  int iMaxTry=SQUISH_LOCK_RETRY;

  /* Only need to lock the area the first time */

  if (Sqd->fLocked++ != 0)
    return TRUE;

  /* The first step is to obtain a lock on the Squish file header.  Another *
   * process may be attempting to do the same thing, so we retry a couple   *
   * of times just in case.                                                 */

  while (iMaxTry-- && mi.haveshare)
  {
    if (lock(Sqd->sfd, 0L, 1L)==0)
      break;

    /* Wait for one second */

    tdelay(1000);
  }

  /* If we could not get exclusive access to the base, report an error */

  if (!iMaxTry)
  {
    msgapierr=MERR_SHARE;
    Sqd->fHaveExclusive=0;
    return FALSE;
  }

  return TRUE;
}


/* Unlock the first byte of the Squish file */

static unsigned near _SquishUnlockBase(HAREA ha)
{
  /* If we have it locked more than once, only unlock on the last call */

  if (--Sqd->fLocked)
    return TRUE;

  /* Unlock the first byte of the file */

  if (mi.haveshare)
    (void)unlock(Sqd->sfd, 0L, 1L);

  return TRUE;
}



/* Obtain exclusive access to this message area.  We need to do this to     *
 * synchronize access to critical fields in the Squish file header.         */

unsigned _SquishExclusiveBegin(HAREA ha)
{
  SQBASE sqb;

  /* We can't open the header for exclusive access more than once */

  if (Sqd->fHaveExclusive)
  {
    msgapierr=MERR_SHARE;
    return FALSE;
  }


  /* Lock the header */

  if (!_SquishLockBase(ha))
    return FALSE;


  /* Obtain an up-to-date copy of the file header */

  if (!_SquishReadBaseHeader(ha, &sqb) ||
      !_SquishCopyBaseToData(ha, &sqb))
  {
    (void)_SquishUnlockBase(ha);
    return FALSE;
  }

  Sqd->fHaveExclusive=TRUE;
  return TRUE;
}


/* Finish exclusive access to the area header.  Sync the base header        *
 * with what we have in memory, then unlock the file.                       */

unsigned _SquishExclusiveEnd(HAREA ha)
{
  SQBASE sqb;
  unsigned rc;

  if (!Sqd->fHaveExclusive)
  {
    msgapierr=MERR_NOLOCK;
    return FALSE;
  }

  /* Copy the in-memory struct to sqb, then write to disk */

  rc=_SquishCopyDataToBase(ha, &sqb) &&
     _SquishWriteBaseHeader(ha, &sqb);

  /* Relinquish access to the base */

  if (!_SquishUnlockBase(ha))
    rc=FALSE;

  Sqd->fHaveExclusive=FALSE;

  return rc;
}


/* Lock this message area for exclusive access */

sword MAPIENTRY SquishLock(HAREA ha)
{
  /* Only need to lock once */

  if (Sqd->fLockFunc++ != 0)
    return 0;

  /* Lock the header */

  if (!_SquishLockBase(ha))
    return -1;


  /* Read the index into memory */

  if (!_SquishBeginBuffer(Sqd->hix))
  {
    (void)_SquishUnlockBase(ha);
    return -1;
  }

  return 0;
}


/* Unlock an area that was opened for exclusive access */

sword MAPIENTRY SquishUnlock(HAREA ha)
{
  if (Sqd->fLockFunc==0)
  {
    msgapierr=MERR_NOLOCK;
    return -1;
  }

  if (--Sqd->fLockFunc != 0)
    return 0;

  (void)_SquishEndBuffer(Sqd->hix);
  (void)_SquishUnlockBase(ha);

  return 0;
}


