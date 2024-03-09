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

/*# name=First-character-of-word-in-string function
*/

#include <stdio.h>
#include <string.h>
#include "prog.h"

/*
main()
{
  char *test="  NORMAL   ";
  char *test2="NORMAL\n";
  char *test3="Sysop";

  printf("1:`%s'\n",firstchar(test," \t\n",2));
  printf("2:`%s'\n",firstchar(test2," \t\n",2));
  printf("3:`%s'\n",firstchar(test3," \t\n",2));
}
*/

char * _fast firstchar(char *strng,char *delim,int findword)
{
  int x,
      isw,
      sl_d,
      sl_s,
      wordno=0;

  char *string,
       *oldstring;

  /* We can't do *anything* if the string is blank... */

  if (! *strng)
    return NULL;

  string=oldstring=strng;

  sl_d=strlen(delim);

  for (string=strng;*string;string++)
  {
    for (x=0,isw=0;x <= sl_d;x++)
      if (*string==delim[x])
        isw=1;

    if (isw==0)
    {
      oldstring=string;
      break;
    }
  }

  sl_s=strlen(string);

  for (wordno=0;(string-oldstring) < sl_s;string++)
  {
    for (x=0,isw=0;x <= sl_d;x++)
      if (*string==delim[x])
      {
        isw=1;
        break;
      }

    if (!isw && string==oldstring)
      wordno++;

    if (isw && (string != oldstring))
    {
      for (x=0,isw=0;x <= sl_d;x++) if (*(string+1)==delim[x])
      {
        isw=1;
        break;
      }

      if (isw==0)
        wordno++;
    }

    if (wordno==findword)
      return((string==oldstring || string==oldstring+sl_s) ? string : string+1);
  }

  return NULL;
}


