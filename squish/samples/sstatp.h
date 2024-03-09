

struct _nodtot
{
  NETADDR node;
  double total_percent_bytes;   /* Amount this node pays of total bill.     */
  double total_percent_msgs;    /* Amount this node pays of total bill.     */
  struct _nodtot *next;
};

struct _stlist
{
  NETADDR node;                 /* Address of this node                     */

  dword out_msgs;               /* Total # of msgs sent to this node        */
  dword out_bytes;              /* Total # of bytes sent to this node       */
  struct _stlist *next;
};

struct _ahlist
{
  char tag[AH_TAGLEN];

  dword in_msgs;
  dword in_bytes;

  dword total_out_bytes;
  dword total_out_msgs;

  struct _stlist *slist;
  struct _ahlist *next;
};

struct _nodelist
{
  NETADDR n;
  struct _nodelist *next;
};

struct _arealist
{
  char *tag;
  struct _arealist *next;
};

static struct _sscfg
{
  struct _nodelist *node;
  struct _arealist *area;
  word do_all;
};

