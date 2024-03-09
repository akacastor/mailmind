#define VERSION "1.1"

#define FALSE 0
#define PATHLEN 120

#define WHERE_TO    0x01
#define WHERE_FROM  0x02
#define WHERE_SUBJ  0x04
#define WHERE_BODY  0x08

struct _srch
{
  byte where;
  char *text;
  struct _srch *next;
};

struct _ka
{
  char *path;
  struct _srch *search;
  struct _ka *next;
};

static char * near ssstrdup(char *s);
static void * near ssmalloc(size_t size);
static word near KillrCatInit(void);
static void near KillrCatTerminate(void);
static word KillrCatConfig(char *name, struct _ka **ka);
static void near KillrCatPounce(struct _ka *k);
static void near KillrCatStalkPrey(struct _ka *k, HAREA harea);
static char * near KillrCatFight(struct _ka *k, XMSG *msg, char *txt);

