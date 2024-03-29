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

/*# name=strftime() function, since BoreLand (heh) doesn't include
    name=one with TC 2.0!
*/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "unistr.h"
#include "prog.h"

#ifndef NO_STRFTIME

/* Note:  TZ environment variable MUST be defined to use the %Z function! */

size_t cdecl strftime(char *string, size_t maxsize, const char *format, const struct tm *current_time)
{
  const char *in;
  
  char *out,
       *scrptr;

  char temp[250];

  maxsize=min(maxsize,230);

  for (in=format,out=temp;*in;in++)
  {
    if ((int)(out-(int)temp) >= maxsize)
      break;

    if (*in=='%')
    {
      switch(*++in)
      {
        case 'a':
          strcpy(out,weekday_ab[current_time->tm_wday]);
          break;

        case 'A':
          strcpy(out,weekday[current_time->tm_wday]);
          break;

        case 'b':
          strcpy(out,months_ab[current_time->tm_mon]);
          break;

        case 'B':
          strcpy(out,months[current_time->tm_mon]);
          break;

        case 'c':
          sprintf(out,"%02d-%02d-%02d %02d:%02d:%02d",
                  current_time->tm_mon+1,
                  current_time->tm_mday,
                  current_time->tm_year,
                  current_time->tm_hour,
                  current_time->tm_min,
                  current_time->tm_sec);
          break;

        case 'd':
          sprintf(out,"%02d",current_time->tm_mday);
          break;

        case 'H':
          sprintf(out,"%02d",current_time->tm_hour);
          break;

        case 'I':
          sprintf(out,"%02d",
                  (current_time->tm_hour >= 0 && current_time->tm_hour <= 12 ?
                   current_time->tm_hour :
                   current_time->tm_hour-12));
          break;

        case 'j':
          sprintf(out,"%03d",current_time->tm_yday+1);
          break;

        case 'm':
          sprintf(out,"%02d",(current_time->tm_mon)+1);
          break;

        case 'M':
          sprintf(out,"%02d",current_time->tm_min);
          break;

        case 'p':
          strcpy(out,(current_time->tm_hour < 12 ? "am" : "pm"));
          break;

        case 'S':
          sprintf(out,"%02d",current_time->tm_sec);
          break;

        case 'U': /* ??? */
          sprintf(out,"%02d",(current_time->tm_yday)/7);
          break;

        case 'w':
          sprintf(out,"%d",current_time->tm_wday);
          break;

        case 'W': /* ??? */
          sprintf(out,"%02d",(current_time->tm_yday)/7);
          break;

        case 'x':
          sprintf(out,"%02d-%02d-%02d",
                  (current_time->tm_mon)+1,
                  current_time->tm_mday,
                  current_time->tm_year);
          break;

        case 'X':
          sprintf(out,"%02d:%02d:%02d",
                  current_time->tm_hour,
                  current_time->tm_min,
                  current_time->tm_sec);
          break;

        case 'y':
          sprintf(out,"%02d",current_time->tm_year % 100);
          break;

        case 'Y':
          sprintf(out,"%02d",current_time->tm_year+1900);
          break;

        case 'Z': /* ??? */

          if ((scrptr=getenv("TZ")) != 0)
          {
            scrptr[3]=0;
            strcpy(out,strupr(scrptr));
          }
          else strcpy(string,"??T");
          break;

        case '%':
          strcpy(out,"%");
          break;
      }

      out += strlen(out);
    }
    else *out++=*in;
  }

  *out='\0';

  strcpy(string,temp);

  return(strlen(string));
}

#endif /* !NO_STRFTIME */

