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
#include <pwd.h>
#include "getopt.h"
#include "command.h"

#include "vtysh.h"
#include "vtysh_user.h"

/* VTY shell program name. */
char *progname;


/* Integrated configuration file. */
char *integrate_file = NULL;
char *integrate_current = NULL;


/* Flag for indicate executing child command. */
int execute_flag = 0;

/* For sigsetjmp() & siglongjmp(). */
static sigjmp_buf jmpbuf;

/* Flag for avoid recursive siglongjmp() call. */
static int jmpflag = 0;



/* SIGTSTP handler.  This function care user's ^Z input. */
void sigtstp (int sig)
{
    /* Check jmpflag for duplicate siglongjmp(). */
    if (! jmpflag)
        return;

    jmpflag = 0;

    /* Back to main command loop. */
    siglongjmp (jmpbuf, 1);
}

/* SIGINT handler.  This function care user's ^Z input.  */
void sigint (int sig)
{
    /* Check this process is not child process. */
    if (! execute_flag)
    {
        printf ("\n");
    }
}

/* Signale wrapper. */
void signal_set (int signo, void (*func)(int))
{
    int ret;
    struct sigaction sig;
    struct sigaction osig;

    sig.sa_handler = func;
    sigemptyset (&sig.sa_mask);
    sig.sa_flags = 0;

    ret = sigaction (signo, &sig, &osig);
}

/* Initialization of signal handles. */
void signal_init ()
{
    signal_set (SIGINT, sigint);
    signal_set (SIGTSTP, sigtstp);
    signal_set (SIGPIPE, SIG_IGN);
}

/* Help information display. */
static void usage (int status)
{
    if (status != 0)
        fprintf (stderr, "Try `%s --help' for more information.\n", progname);
    else
    {
      printf ("Usage : %s [OPTION...]\n\n\
Daemon which manages kernel routing table management and \
redistribution between different routing protocols.\n\n\
-b, --boot               Execute boot startup configuration\n\
-e, --eval               Execute argument as command\n\
-h, --help               Display this help and exit\n\
\n", progname);
    }
    exit (status);
}

/* VTY shell options, we use GNU getopt library. */
struct option longopts[] = 
{
    { "boot",                no_argument,             NULL, 'b'},
    { "eval",                 required_argument,       NULL, 'e'},
    { "help",                 no_argument,             NULL, 'h'},
    { 0 }
};

void vty_main_loop();

/* VTY shell main routine. */
int main (int argc, char **argv, char **env)
{
    char *p;
    int opt;
    int eval_flag = 0;
    int boot_flag = 0;
    char *eval_line = NULL;
    char *integrated_file = NULL;

    /* Preserve name of myself. */
    progname = ((p = strrchr (argv[0], '/')) ? ++p : argv[0]);
    //zlog_default = openzlog (progname, ZLOG_STDOUT, ZLOG_ZEBRA,LOG_CONS|LOG_NDELAY|LOG_PID, LOG_DAEMON);

    /* Option handling. */
    while (1) 
    {
        opt = getopt_long (argc, argv, "be:h", longopts, 0);

        if (opt == EOF)
        break;

        switch (opt) 
        {
            case 0:
                break;
            case 'b':
                boot_flag = 1;
                break;
            case 'e':
                eval_flag = 1;
                eval_line = optarg;
                break;
            case 'h':
                usage (0);
                break;
            case 'i':
                integrated_file = strdup (optarg);
            default:
                usage (1);
                break;
         }
     }


    /* Signal and others. */
    signal_init ();

    /* Make vty structure and register commands. */
    vtysh_init_vty ();

    vtysh_user_init ();


    vty_init_vtysh ();

    sort_node ();



    vty_hello (vty);

    vtysh_auth ();

    /* Preparation for longjmp() in sigtstp(). */
    sigsetjmp (jmpbuf, 1);
    jmpflag = 1;


    /* Main command loop. */
    vty_main_loop();
    printf ("\n");

    /* Rest in peace. */
    exit (0);
}
