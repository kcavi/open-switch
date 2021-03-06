/* User authentication for vtysh.
 * Copyright (C) 2000 Kunihiro Ishiguro
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

#include <pwd.h>

#include "memory.h"
#include "linklist.h"
#include "command.h"


struct user
{
  char *name;
  u_char nopassword;
};

struct list *userlist;

struct user *
user_new ()
{
  struct user *user;
  user = XMALLOC (0, sizeof (struct user));
  memset (user, 0, sizeof (struct user));
  return user;
}

void
user_free (struct user *user)
{
  XFREE (0, user);
}

struct user *
user_lookup (char *name)
{
  struct listnode *nn;
  struct user *user;

  LIST_LOOP (userlist, user, nn)
    {
      if (strcmp (user->name, name) == 0)
	return user;
    }
  return NULL;
}

void
user_config_write ()
{
  struct listnode *nn;
  struct user *user;

  LIST_LOOP (userlist, user, nn)
    {
      if (user->nopassword)
	printf (" username %s nopassword\n", user->name);
    }
}

struct user *
user_get (char *name)
{
  struct user *user;
  user = user_lookup (name);
  if (user)
    return user;

  user = user_new ();
  user->name = strdup (name);
  listnode_add (userlist, user);

  return user;
}

DEFUN (username_nopassword,
       username_nopassword_cmd,
       "username WORD nopassword",
       "\n"
       "\n"
       "\n")
{
  struct user *user;
  user = user_get (argv[0]);
  user->nopassword = 1;
  return CMD_SUCCESS;
}

int
vtysh_auth ()
{
  struct user *user;
  struct passwd *passwd;

  passwd = getpwuid (geteuid ());

  user = user_lookup (passwd->pw_name);
  if (user && user->nopassword)
    /* Pass through */;
  else
    {
    }
  return 0;
}

void
vtysh_user_init ()
{
  userlist = list_new ();
  install_element (CONFIG_NODE, &username_nopassword_cmd);
}
