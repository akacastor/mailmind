/* Comments?  What comments?  :-) */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "msgapi.h"
#include "killrcat.h"

#ifndef OS_2
#include "prog.h"
#else
char *firstchar(char *strng,char *delim,int findword)
{
  int x,
      isw,
      sl_d,
      sl_s,
      wordno=0;

  char *string,
       *oldstring;

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


char * Strip_Trailing(char *str,char strip)
{
  int x;

  if (str && *str && str[x=strlen(str)-1]==strip)
    str[x]='\0';

  return str;
}

#define ISLEFT(c) ((c) > (byte)0x80 && (c) < (byte)0xff)
#define ISRIGHT(c) (((c) >= (byte)0x40 && (c) <= (byte)0x7e) || \
                    ((c) >= (byte)0xa1 && (c) <= (byte)0xfe))

word ischin(byte *buf)
{
  return (ISLEFT(buf[0]) && ISRIGHT(buf[1]));
}

char * stristr(char *string,char *search)
{
  word last_found=0;
  word strlen_search=strlen(search);
  byte l1, l2;
  word i;

  if (string)
  {
    while (*string)
    {
      l1=(byte)(ischin(string) ? 2 : 1);
      l2=(byte)(ischin(search+last_found) ? 2 : 1);

      if (l1==l2)
        i=(l1==1) ? memicmp(string, search+last_found, l1) : 1;
      else i=1;
      
      if (!i)
        last_found += l1;
      else
      {
        if (last_found != 0)
        {
          string -= last_found-1;
          last_found=0;
          continue;
        }
      }

      string += l1;

      if (last_found==strlen_search) return(string-last_found);
    }
  }

  return(NULL);
}
#endif

static void near KillrCatPounce(struct _ka *k)
{
  HAREA harea;
  char *p;

  p=k->path;

  if (*p=='$')
    p++;

  if ((harea=MsgOpenArea(p, MSGAREA_NORMAL,
                         *k->path=='$' ? MSGTYPE_SQUISH : MSGTYPE_SDM))==NULL)
  {
    printf("Can't open message area `%s'!\n", p);
    return;
  }

  printf("Scanning %s:\n", p);
  
  MsgLock(harea);

  KillrCatStalkPrey(k, harea);

  MsgCloseArea(harea);
}


static void near KillrCatStalkPrey(struct _ka *k, HAREA harea)
{
  char *txt, *found;
  HMSG hmsg;
  XMSG msg;
  long mn;

  for (mn=1; (dword)mn <= MsgGetHighMsg(harea); mn++)
  {
    if ((hmsg=MsgOpenMsg(harea, MOPEN_READ, mn))==NULL)
      continue;
    
    if ((txt=malloc((size_t)MsgGetTextLen(hmsg))) != NULL)
    {
      MsgReadMsg(hmsg, &msg, 0L, MsgGetTextLen(hmsg), txt, 0L, NULL);

      if ((found=KillrCatFight(k, &msg, txt)) != NULL)
      {
        MsgCloseMsg(hmsg);
        MsgKillMsg(harea, mn);

        printf("Nuked #%ld - Found `%s'\n", mn, found);
      }

      free(txt);

      if (found)
      {
        mn--;
        continue;
      }
    }

    MsgCloseMsg(hmsg);
  }
}


static char * near KillrCatFight(struct _ka *k, XMSG *msg, char *txt)
{
  struct _srch *sr;

  for (sr=k->search; sr; sr=sr->next)
  {
    if ((sr->where & WHERE_TO) && stristr(msg->to, sr->text))
      return msg->to;

    if ((sr->where & WHERE_FROM) && stristr(msg->from, sr->text))
      return msg->from;

    if ((sr->where & WHERE_SUBJ) && stristr(msg->subj, sr->text))
      return msg->subj;

    if ((sr->where & WHERE_BODY) && stristr(txt, sr->text))
      return sr->text;
  }

  return NULL;
}

static word near KillrCatInit(void)
{
  struct _minf mi;

  memset(&mi, '\0', sizeof mi);
  mi.def_zone=1;
  mi.req_version=MSGAPI_VERSION;

  return (MsgOpenApi(&mi)==0);
}


static word KillrCatConfig(char *name, struct _ka **ka)
{
  char line[PATHLEN];
  char orig[PATHLEN];
  static char delim[]=" \t\r\n";
  struct _ka *k;
  struct _srch *sr;
  word state=0;
  FILE *fp;
  char *s, *p;

  *ka=NULL;

  if ((fp=fopen(name, "r"))==NULL)
  {
    printf("Error opening `%s'!\n", name);
    return FALSE;
  }
  
  while (fgets(line, PATHLEN, fp))
  {
    strcpy(orig, line);

    if (*line==';')
      continue;

    if (state==0)
    {
      if (*line=='\n')
        continue;

      s=strtok(line, delim);

      if (!line || stricmp(line, "area") != 0)
      {
        printf("Invalid line: `%s'\n", line ? line : "");
        continue;
      }

      if ((s=strtok(NULL, delim))==NULL)
      {
        printf("Ran out of memory!\n");
        exit(1);
      }

      k=ssmalloc(sizeof(*k));
      k->path=ssstrdup(s);
      k->next=*ka;
      *ka=k;

      state=1;
    }
    else
    {
      if (*line=='\n')
      {
        state=0;
        continue;
      }

      Strip_Trailing(orig, '\n');

      if ((s=strtok(line, delim)) != NULL)
      {
        sr=ssmalloc(sizeof(*sr));

        for (p=s; *p; p++)
          switch (toupper(*p))
          {
            case 'T': sr->where |= WHERE_TO;    break;
            case 'F': sr->where |= WHERE_FROM;  break;
            case 'S': sr->where |= WHERE_SUBJ;  break;
            case 'B': sr->where |= WHERE_BODY;  break;
          }

        if ((s=firstchar(orig, delim, 2)) != NULL)
          sr->text=ssstrdup(s);

        sr->next=k->search;
        k->search=sr;
      }
    }
  }
  
  fclose(fp);
  return 0;
}

static void near KillrCatTerminate(void)
{
  MsgCloseApi();
}



static char * near ssstrdup(char *s)
{
  char *str;

  if ((str=strdup(s))==NULL)
  {
    puts("Not enough memory for strdup!");
    exit(1);
  }

  return str;
}

static void * near ssmalloc(size_t size)
{
  void *p;

  if ((p=malloc(size))==NULL)
  {
    puts("Not enough memory for malloc!");
    exit(1);
  }

  memset(p, '\0', size);

  return p;
}



int _stdc main(int argc, char *argv[])
{
  struct _ka *ka, *k;
  
  (void)argc;

  printf("\nKILLRCAT  Penultimate Post Pulverizer, Version " VERSION ".\n");
  printf("Copyright 1990-94 by Scott J. Dudley.  All rights reserved.\n\n");
  
  if (!KillrCatInit())
    return 1;

  KillrCatConfig(argv[1] ? argv[1] : "killrcat.cfg", &ka);

  for (k=ka; k; k=k->next)
    KillrCatPounce(k);

  KillrCatTerminate();
  return 0;
}

