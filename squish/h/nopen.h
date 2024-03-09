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

#ifndef __NOPEN_H_DEFINED
#define __NOPEN_H_DEFINED

#include "uni.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "prog.h"

#ifdef __TURBOC__
#undef sopen
int _stdc sopen(const char *name, int mode, int shacc, ...);
#endif

int _stdc nsopen(const char *name, int mode, int shacc, ...);
int _stdc nopen(const char *name, int mode, ...);
int _stdc nread(int fd, char *buf, unsigned len);
int _stdc nwrite(int fd, char *buf, unsigned len);
long _stdc nlseek(int fd, long ofs, int pos);
long _stdc ntell(int fd);
int _stdc nclose(int fd);
int _stdc ndup(int fd);
int _stdc ndup2(int fd1, int fd2);

#endif /* __NOPEN_H_DEFINED */

