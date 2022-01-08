/* Zebra common header.
   Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002 Kunihiro Ishiguro

This file is part of GNU Zebra.

GNU Zebra is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

GNU Zebra is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Zebra; see the file COPYING.  If not, write to the Free
Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

#ifndef _ZEBRA_H
#define _ZEBRA_H
#define MINISWTICH_VERSION     "0.0.1"



#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <syslog.h>
#include <time.h>
#include <sys/uio.h>
#include <sys/utsname.h>
#include <stdarg.h>
#include <assert.h>

/* network include group */
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <net/route.h>
#include <arpa/inet.h>
#include <arpa/telnet.h>



#endif /* _ZEBRA_H */
