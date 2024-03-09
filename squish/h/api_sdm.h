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

/* $Id: api_sdm.h 1.1 1993/12/05 05:10:05 sjd Exp $ */

#ifndef __API_SDM_H_DEFINED
#define __API_SFM_H_DEFINED


#define MAX_SDM_CLEN  512   /* Maximum number of bytes which can be used    *
                             * for kludge lines at top of *.MSG-type        *
                             * messages.                                    */


struct _msgh
{
  HAREA ha;
  dword id;      /* Must always equal MSGH_ID */

  dword bytes_written;
  dword cur_pos;

  /* For *.MSG only! */

  sdword clen;
  byte *ctrl;
  dword msg_len;
  dword msgtxt_start;
  word zplen;
  int fd;
};


/*************************************************************************/
/* This junk is unique to *.MSG!       NO APPLICATIONS SHOULD USE THESE! */
/*************************************************************************/

struct _sdmdata
{
  byte base[80];
  
  unsigned *msgnum;   /* has to be of type 'int' for qksort() fn */
  unsigned msgnum_len;
    
  dword hwm;
  word hwm_chgd;
  
  unsigned msgs_open;
};




#endif /* __API_SDM_H_DEFINED */


