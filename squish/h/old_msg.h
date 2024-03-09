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

#ifndef __OLD_MSG_H_DEFINED
#define __OLD_MSG_H_DEFINED

/*--------------------------------------------------------------------------*/
/* Message header                                                           */
/*--------------------------------------------------------------------------*/
struct _omsg
   {
      byte from[36];
      byte to[36];
      byte subj[72];
      byte date[20];       /* Obsolete/unused ASCII date information        */
/**/  word times;          /* FIDO<tm>: Number of times read                */
      sword dest;          /* Destination node                              */
      sword orig;          /* Origination node number                       */
/**/  word cost;           /* Unit cost charged to send the message         */

      sword orig_net;      /* Origination network number                    */
      sword dest_net;      /* Destination network number                    */

                           /* A TIMESTAMP is a 32-bit integer in the Unix   */
                           /* flavor (ie. the number of seconds since       */
                           /* January 1, 1970).  Timestamps in messages are */
                           /* always Greenwich Mean Time, never local time. */

      struct _stamp date_written;   /* When user wrote the msg              */
      struct _stamp date_arrived;   /* When msg arrived on-line             */

      word reply;          /* Current msg is a reply to this msg number     */
      word attr;           /* Attribute (behavior) of the message           */
      word up;             /* Next message in the thread                    */
   };

#endif

