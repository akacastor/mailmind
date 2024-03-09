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

/* $Id: api_sdmp.h 1.1 1993/12/05 05:10:05 sjd Exp $ */

static sword MAPIENTRY SdmCloseArea(HAREA mh);
static HMSG  MAPIENTRY SdmOpenMsg(HAREA mh,word mode,dword msgnum);
static sword MAPIENTRY SdmCloseMsg(HMSG msgh);
static dword MAPIENTRY SdmReadMsg(HMSG msgh,PXMSG msg,dword offset,dword bytes,byte *text,dword clen,byte *ctxt);
static sword MAPIENTRY SdmWriteMsg(HMSG msgh,word append,PXMSG msg,byte *text,dword textlen,dword totlen,dword clen,byte *ctxt);
static sword MAPIENTRY SdmKillMsg(HAREA mh,dword msgnum);
static sword MAPIENTRY SdmLock(HAREA mh);
static sword MAPIENTRY SdmUnlock(HAREA mh);
static sword MAPIENTRY SdmSetCurPos(HMSG msgh,dword pos);
static dword MAPIENTRY SdmGetCurPos(HMSG msgh);
static UMSGID MAPIENTRY SdmMsgnToUid(HAREA mh,dword msgnum);
static dword MAPIENTRY SdmUidToMsgn(HAREA mh,UMSGID umsgid,word type);
static dword MAPIENTRY SdmGetHighWater(HAREA mh);
static sword MAPIENTRY SdmSetHighWater(HAREA sq,dword hwm);
static dword MAPIENTRY SdmGetTextLen(HMSG msgh);
static dword MAPIENTRY SdmGetCtrlLen(HMSG msgh);
static UMSGID MAPIENTRY SdmGetNextUid(HAREA ha);

static void Convert_Fmsg_To_Xmsg(struct _omsg *fmsg,PXMSG msg,word def_zone);
static void Convert_Xmsg_To_Fmsg(PXMSG msg,struct _omsg *fmsg);
static void Init_Xmsg(PXMSG msg);
static sword near _SdmRescanArea(HAREA mh);
static sword near _Grab_Clen(HMSG msgh);
static void MAPIENTRY WriteToFd(byte OS2FAR *str);
static void near Get_Binary_Date(struct _stamp *todate,struct _stamp *fromdate,byte *asciidate);


static int statfd; /* file handle for WriteToFd */
static byte *sd_msg="%s%u.msg";

/* Pointer to 'struct _sdmdata' so we can get Turbo Debugger to use         *
 * the _sdmdata structure...                                                */

#ifdef __TURBOC__
static struct _sdmdata *_junksqd;
#endif

static struct _apifuncs sdm_funcs=
{
  SdmCloseArea,
  SdmOpenMsg,
  SdmCloseMsg,
  SdmReadMsg,
  SdmWriteMsg,
  SdmKillMsg,
  SdmLock,
  SdmUnlock,
  SdmSetCurPos,
  SdmGetCurPos,
  SdmMsgnToUid,
  SdmUidToMsgn,
  SdmGetHighWater,
  SdmSetHighWater,
  SdmGetTextLen,
  SdmGetCtrlLen,
  SdmGetNextUid
};

