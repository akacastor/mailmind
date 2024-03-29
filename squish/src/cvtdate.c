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

#include <time.h>
#include "prog.h"

static int is_dst=-1;


/* Find out the current status of daylight savings time */

static void near InitCvt(void)
{
  time_t tnow;

  tnow=time(NULL);

  is_dst=!! (localtime(&tnow)->tm_isdst);
}


/* Convert a DOS-style bitmapped date into a 'struct tm'-type date. */

struct tm * _fast DosDate_to_TmDate(union stamp_combo *dosdate,
                                     struct tm *tmdate)
{
  if (is_dst==-1)
    InitCvt();

  tmdate->tm_mday=dosdate->msg_st.date.da;
  tmdate->tm_mon =dosdate->msg_st.date.mo-1;
  tmdate->tm_year=dosdate->msg_st.date.yr+80;

  tmdate->tm_hour=dosdate->msg_st.time.hh;
  tmdate->tm_min =dosdate->msg_st.time.mm;
  tmdate->tm_sec =dosdate->msg_st.time.ss << 1;

  tmdate->tm_isdst=is_dst;

  return tmdate;
}

/* Convert a 'struct tm'-type date into an Opus/DOS bitmapped date */

union stamp_combo * _fast TmDate_to_DosDate(struct tm *tmdate,
                                             union stamp_combo *dosdate)
{
  dosdate->msg_st.date.da=tmdate->tm_mday;
  dosdate->msg_st.date.mo=tmdate->tm_mon+1;
  dosdate->msg_st.date.yr=(tmdate->tm_year < 80) ? 0 : tmdate->tm_year-80;

  dosdate->msg_st.time.hh=tmdate->tm_hour;
  dosdate->msg_st.time.mm=tmdate->tm_min;
  dosdate->msg_st.time.ss=tmdate->tm_sec >> 1;

  return dosdate;
}



char * _fast sc_time(union stamp_combo *sc,char *string)
{
  if (sc->msg_st.date.yr==0)
    *string='\0';
  else
  {
    sprintf(string,
           "%2d %s %d %02d:%02d:%02d",
           sc->msg_st.date.da,
           months_ab[sc->msg_st.date.mo-1],
           80+sc->msg_st.date.yr,
           sc->msg_st.time.hh,
           sc->msg_st.time.mm,
           sc->msg_st.time.ss << 1);
  }

  return string;
}

