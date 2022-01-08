/*
 * Memory management routine
 * Copyright (C) 1998 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.  
 */

#include <common.h>

#include "log.h"
#include "memory.h"

void alloc_inc (int);
void alloc_dec (int);

struct message mstr [] =
{
  { MTYPE_THREAD, "thread" },
  { MTYPE_THREAD_MASTER, "thread_master" },
  { MTYPE_VECTOR, "vector" },
  { MTYPE_VECTOR_INDEX, "vector_index" },
  { MTYPE_IF, "interface" },
  { 0, NULL },
};

/* Fatal memory allocation error occured. */
static void
zerror (const char *fname, int type, size_t size)
{
  fprintf (stderr, "%s : can't allocate memory for `%s' size %d\n", 
	   fname, lookup (mstr, type), (int) size);
  exit (1);
}

/* Memory allocation. */
void *
zmalloc (int type, size_t size)
{
  void *memory;

  memory = malloc (size);

  if (memory == NULL)
    zerror ("malloc", type, size);

  alloc_inc (type);

  return memory;
}

/* Memory allocation with num * size with cleared. */
void *
zcalloc (int type, size_t size)
{
  void *memory;

  memory = calloc (1, size);

  if (memory == NULL)
    zerror ("calloc", type, size);

  alloc_inc (type);

  return memory;
}

/* Memory reallocation. */
void *
zrealloc (int type, void *ptr, size_t size)
{
  void *memory;

  memory = realloc (ptr, size);
  if (memory == NULL)
    zerror ("realloc", type, size);
  return memory;
}

/* Memory free. */
void
zfree (int type, void *ptr)
{
  alloc_dec (type);
  free (ptr);
}

/* String duplication. */
char *
zstrdup (int type, char *str)
{
  void *dup;

  dup = strdup (str);
  if (dup == NULL)
    zerror ("strdup", type, strlen (str));
  alloc_inc (type);
  return dup;
}


struct 
{
  char *name;
  unsigned long alloc;
} mstat [MTYPE_MAX];

/* Increment allocation counter. */
void
alloc_inc (int type)
{
  mstat[type].alloc++;
}

/* Decrement allocation counter. */
void
alloc_dec (int type)
{
  mstat[type].alloc--;
}

/* Looking up memory status from vty interface. */
#include "vector.h"
#include "vty.h"
#include "command.h"

/* For pretty printng of memory allocate information. */
struct memory_list
{
  int index;
  char *format;
};

struct memory_list memory_list_lib[] =
{
  { MTYPE_TMP,                "Temporary memory" },
  { MTYPE_ROUTE_TABLE,        "Route table     " },
  { MTYPE_ROUTE_NODE,         "Route node      " },
  { MTYPE_RIB,                "RIB             " },
  { MTYPE_NEXTHOP,            "Nexthop         " },
  { MTYPE_LINK_LIST,          "Link List       " },
  { MTYPE_LINK_NODE,          "Link Node       " },
  { MTYPE_HASH,               "Hash            " },
  { MTYPE_HASH_BACKET,        "Hash Bucket     " },
  { MTYPE_ACCESS_LIST,        "Access List     " },
  { MTYPE_ACCESS_LIST_STR,    "Access List Str " },
  { MTYPE_ACCESS_FILTER,      "Access Filter   " },
  { MTYPE_PREFIX_LIST,        "Prefix List     " },
  { MTYPE_PREFIX_LIST_STR,    "Prefix List Str " },
  { MTYPE_PREFIX_LIST_ENTRY,  "Prefix List Entry "},
  { MTYPE_ROUTE_MAP,          "Route map       " },
  { MTYPE_ROUTE_MAP_NAME,     "Route map name  " },
  { MTYPE_ROUTE_MAP_INDEX,    "Route map index " },
  { MTYPE_ROUTE_MAP_RULE,     "Route map rule  " },
  { MTYPE_ROUTE_MAP_RULE_STR, "Route map rule str" },
  { MTYPE_DESC,               "Command desc    " },
  { MTYPE_BUFFER,             "Buffer          " },
  { MTYPE_BUFFER_DATA,        "Buffer data     " },
  { MTYPE_STREAM,             "Stream          " },
  { MTYPE_KEYCHAIN,           "Key chain       " },
  { MTYPE_KEY,                "Key             " },
  { MTYPE_VTY,                "VTY             " },
  { -1, NULL }
};


struct memory_list memory_list_separator[] =
{
  { 0, NULL},
  {-1, NULL}
};

void
show_memory_vty (struct vty *vty, struct memory_list *list)
{
  struct memory_list *m;

  for (m = list; m->index >= 0; m++)
    if (m->index == 0)
      vty_out (vty, "-----------------------------\r\n");
    else
      vty_out (vty, "%-22s: %5ld\r\n", m->format, mstat[m->index].alloc);
}

DEFUN (show_memory_all,
       show_memory_all_cmd,
       "show memory all",
       "Show running system information\n"
       "Memory statistics\n"
       "All memory statistics\n")
{
  show_memory_vty (vty, memory_list_lib);

  return CMD_SUCCESS;
}

ALIAS (show_memory_all,
       show_memory_cmd,
       "show memory",
       "Show running system information\n"
       "Memory statistics\n");

DEFUN (show_memory_lib,
       show_memory_lib_cmd,
       "show memory lib",
       SHOW_STR
       "Memory statistics\n"
       "Library memory\n")
{
  show_memory_vty (vty, memory_list_lib);
  return CMD_SUCCESS;
}



void
memory_init ()
{
  install_element (VIEW_NODE, &show_memory_cmd);
  install_element (VIEW_NODE, &show_memory_all_cmd);
  install_element (VIEW_NODE, &show_memory_lib_cmd);


  install_element (ENABLE_NODE, &show_memory_cmd);
  install_element (ENABLE_NODE, &show_memory_all_cmd);
  install_element (ENABLE_NODE, &show_memory_lib_cmd);

}
