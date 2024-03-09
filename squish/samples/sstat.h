#include "msgapi.h"
#include "typedefs.h"
#include "sstat2.h"


/* Linked list of statistics information which parallels the linked         *
 * list of nodes to scan, 'scan[]'.  Used internally by sstat               */

struct _statlist
{
  NETADDR node;                 /* Address of this node                     */

  dword out_msgs;               /* Total # of msgs sent to this node        */
  dword out_bytes;              /* Total # of bytes sent to this node       */
  struct _statlist *next;
};

