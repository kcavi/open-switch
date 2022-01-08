/* Virtual terminal interface shell.
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

#include <sys/un.h>
#include <setjmp.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include "command.h"
#include "memory.h"
#include "vtysh.h"

/* Struct VTY. */
struct vty *vty;

/* result of cmd_complete_command() call will be stored here
   and used in new_completion() in order to put the space in
   correct places only */
int complete_status;


struct cmd_node interface_node =
{
  INTERFACE_NODE,
  "%s(config-if)# ",
};

/* Execute command in child process. */
int
execute_command (char *command, int argc, char *arg1, char *arg2)
{
  int ret;
  pid_t pid;
  int status;

  /* Call fork(). */
  pid = fork ();

  if (pid < 0)
    {
      /* Failure of fork(). */
      fprintf (stderr, "Can't fork: %s\n", strerror (errno));
      exit (1);
    }
  else if (pid == 0)
    {
      /* This is child process. */
      switch (argc)
	{
	case 0:
	  ret = execlp (command, command, NULL);
	  break;
	case 1:
	  ret = execlp (command, command, arg1, NULL);
	  break;
	case 2:
	  ret = execlp (command, command, arg1, arg2, NULL);
	  break;
	}

      /* When execlp suceed, this part is not executed. */
      fprintf (stderr, "Can't execute %s: %s\n", command, strerror (errno));
      exit (1);
    }
  else
    {
      /* This is parent. */
      execute_flag = 1;
      ret = wait4 (pid, &status, 0, NULL);
      execute_flag = 0;
    }
  return 0;
}

DEFUN (vtysh_ping,
       vtysh_ping_cmd,
       "ping WORD",
       "send echo messages\n"
       "Ping destination address or hostname\n")
{
  execute_command ("ping", 1, argv[0], NULL);
  return CMD_SUCCESS;
}

DEFUN (vtysh_traceroute,
       vtysh_traceroute_cmd,
       "traceroute WORD",
       "Trace route to destination\n"
       "Trace route to destination address or hostname\n")
{
  execute_command ("traceroute", 1, argv[0], NULL);
  return CMD_SUCCESS;
}

DEFUN (vtysh_telnet,
       vtysh_telnet_cmd,
       "telnet WORD",
       "Open a telnet connection\n"
       "IP address or hostname of a remote system\n")
{
  execute_command ("telnet", 1, argv[0], NULL);
  return CMD_SUCCESS;
}

DEFUN (vtysh_ls,
      vtysh_ls_cmd,
      "ls",
      "list file\n"
      "list current file\n")
{
    execute_command ("ls", 1, "-l", NULL);
    return CMD_SUCCESS;
}


DEFUN (vtysh_telnet_port,
       vtysh_telnet_port_cmd,
       "telnet WORD PORT",
       "Open a telnet connection\n"
       "IP address or hostname of a remote system\n"
       "TCP Port number\n")
{
  execute_command ("telnet", 2, argv[0], argv[1]);
  return CMD_SUCCESS;
}

DEFUN (vtysh_start_shell,
       vtysh_start_shell_cmd,
       "start-shell",
       "Start UNIX shell\n")
{
  execute_command ("sh", 0, NULL, NULL);
  return CMD_SUCCESS;
}

DEFUN (vtysh_start_bash,
       vtysh_start_bash_cmd,
       "start-shell bash",
       "Start UNIX shell\n"
       "Start bash\n")
{
  execute_command ("bash", 0, NULL, NULL);
  return CMD_SUCCESS;
}

DEFUN (vtysh_start_zsh,
       vtysh_start_zsh_cmd,
       "start-shell zsh",
       "Start UNIX shell\n"
       "Start Z shell\n")
{
  execute_command ("zsh", 0, NULL, NULL);
  return CMD_SUCCESS;
}

void vtysh_init_vty ()
{
    /* Make vty structure. */
    vty = vty_new ();
    vty->type = VTY_TERM;
    vty->node = VIEW_NODE;

    /* Initialize commands. */
    cmd_init (1);

    /* Install nodes. */
    install_node (&interface_node, NULL);


    install_element (VIEW_NODE, &vtysh_ping_cmd);
    install_element (VIEW_NODE, &vtysh_traceroute_cmd);
    install_element (VIEW_NODE, &vtysh_telnet_cmd);
    install_element (VIEW_NODE, &vtysh_telnet_port_cmd);
    install_element (VIEW_NODE, &vtysh_ls_cmd);

    install_element (ENABLE_NODE, &vtysh_ping_cmd);
    install_element (ENABLE_NODE, &vtysh_traceroute_cmd);
    install_element (ENABLE_NODE, &vtysh_telnet_cmd);
    install_element (ENABLE_NODE, &vtysh_telnet_port_cmd);
    install_element (ENABLE_NODE, &vtysh_start_shell_cmd);
    install_element (ENABLE_NODE, &vtysh_start_bash_cmd);
    install_element (ENABLE_NODE, &vtysh_start_zsh_cmd);
   nm_if_init();

}
