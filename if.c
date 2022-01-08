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

#define MAX_ETH_PORT 12

struct 
{
    int unit;
    int admin_status;
    int oper_status;
    char name[64];
    char desc[64];
    int mtu;
    int negotiation;
    int duplex;
    int linktype;
    int speed;
    int flowctrl;
    int def_vlan;
    char vlanlist[512];
    char tagvlanlist[512];
}eth_port[MAX_ETH_PORT];




DEFUN(show_interface,
    show_interface_cmd,
    "show interface",
    SHOW_STR
    "The information of specify interface\n")
{
    int ret = 0;
    char if_str[128] = {0};
    char descr[128 + 2] = {0};
    int ifOper = 0;
    int ifAdmin = 0;
    int i = 0;
    int flag = 0;
    char state_str[32] = {0};

    while(i < MAX_ETH_PORT)
    {
        snprintf(state_str,sizeof(state_str)-1,"%s/%s",
            (eth_port[i].admin_status== 1)?"up":"down",
            (eth_port[i].oper_status == 1)?"up":"down");

        if(flag == 0)
        {
            vty_out(vty,"  %-18s%-12s%-10s%s%s","Interface","State(a/o)","Mode","Descr",VTY_NEWLINE);
            flag = 1;
        }

        ret = vty_out(vty,"  %-18s%-12s%-10s%s%s",eth_port[i].name,state_str,"bridge",
            eth_port[i].desc,VTY_NEWLINE);
        i++;
    }
    return CMD_SUCCESS;
}


DEFUN(config_one_if,
    config_one_if_cmd,
    "interface (ethernet|fastethernet|gigaethernet) <1-12>",
    "The information of specify interface\n"
    "(ethernet|fastethernet|gigaethernet)")
{
    int ifType = 0;
    int ifIndex = atoi(argv[1]);

    vty->ifindex = ifIndex;
    vty->node = INTERFACE_NODE;

    return CMD_SUCCESS;
}

DEFUN(interface_desc,
    interface_desc_cmd,
    "description DESCR",
    "Set interface description\n"
    "Description, the max length is 64\n")
{
    int ifIndex = vty->ifindex;
    if(argc > 0)
    {
        if(strlen(argv[0]) > 64)
        {
            vty_out(vty, "  %%Failed.alias must be no more than %d.%s", 64,VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    strncpy(eth_port[ifIndex - 1].desc,argv[0],sizeof(eth_port[ifIndex - 1].desc));

    return CMD_SUCCESS;
}

DEFUN(interface_mtu,
    interface_mtu_cmd,
    "mtu <60-10000>",
    "The interface Maximum Transmission Unit (MTU)\n"
    "Size in bytes , default value is 1522\n")
{
    int mtu = 1522;
    int ifIndex = vty->ifindex;
    if(argc > 0)
    {
        mtu = atoi(argv[0]);
    }

    eth_port[ifIndex - 1].mtu = mtu;

    return CMD_SUCCESS;
}


DEFUN(shutdown_if,
    shutdown_if_cmd,
    "shutdown",
    "Shutdown the interface\n")
{
    int ifIndex = vty->ifindex;

    eth_port[ifIndex - 1].admin_status = 0;
    eth_port[ifIndex - 1].oper_status = 0;

    return CMD_SUCCESS;
}

DEFUN(no_shutdown_if,
    no_shutdown_if_cmd,
    "no shutdown",
    NO_STR) 
{
    int ifIndex = vty->ifindex;

    eth_port[ifIndex - 1].admin_status = 1;
    eth_port[ifIndex - 1].oper_status = 1;

    return CMD_SUCCESS;
}

DEFUN(interface_duplex_mode,
    interface_duplex_mode_cmd, 
    "duplex (full|half)",
    "Configure duplex mode\n")
{
    int ifIndex = vty->ifindex;

    if(argv[0][0] == 'f' || argv[0][0] == 'F')
        eth_port[ifIndex - 1].duplex = 1;
    else
        eth_port[ifIndex - 1].duplex = 0;

    return CMD_SUCCESS;

}


DEFUN(interface_ethif_speed ,
    interface_ethif_speed_cmd,
    "speed (10|100|1000|10000)",
    "Set the speed of interface\n")
{
    int ifIndex = vty->ifindex;

    eth_port[ifIndex - 1].speed = atoi(argv[0]);

    return CMD_SUCCESS;

}

DEFUN(interface_flow_ctrl,
    interface_flow_ctrl_cmd,
    "flow-control (enable|disable)",
    "Flow-control of interface\n")
{     
    int ifIndex = vty->ifindex;
    if(argv[0][0] == 'e' || argv[0][0] == 'E')
        eth_port[ifIndex - 1].flowctrl = 1;
    else
        eth_port[ifIndex - 1].flowctrl = 0;
    
    return CMD_SUCCESS;

}

DEFUN(interface_negotiation_enable,
    interface_negotiation_enable_cmd, 
    "negotiation (enable|disable)",
    "Negotiation of interface \n")
{     
    int ifIndex = vty->ifindex;
    if(argv[0][0] == 'e' || argv[0][0] == 'E')
        eth_port[ifIndex - 1].negotiation = 1;
    else
        eth_port[ifIndex - 1].negotiation = 0;

    return CMD_SUCCESS;

}

DEFUN(set_port_type,
    set_port_type_cmd,
    "port link-type (access|trunk|hybrid)",
    "Set the linktype of port\n")
{
    int ifIndex = vty->ifindex;
    if(argv[0][0] == 'a' || argv[0][0] == 'A')
        eth_port[ifIndex - 1].linktype = 0;
    else if(argv[0][0] == 't' || argv[0][0] == 'T')
        eth_port[ifIndex - 1].linktype = 1;
    else
        eth_port[ifIndex - 1].linktype = 2;

    return CMD_SUCCESS;

}

DEFUN(add_access_port_vlan,
    add_access_port_vlan_cmd,
    "port default vlan <1-4094>",
    "Vlan property of port\n")
{
    int ifIndex = vty->ifindex;

    eth_port[ifIndex - 1].def_vlan = atoi(argv[0]);

    return CMD_SUCCESS;

}


void nm_if_init()
{
    int i;
    for(i = 0;i < MAX_ETH_PORT;i++)
    {
        eth_port[i].unit = i+4096;
        eth_port[i].admin_status = 1;
        eth_port[i].oper_status = 1;
        eth_port[i].def_vlan = 1;
        sprintf(eth_port[i].name,"ge1/0/%d",i+1);
        sprintf(eth_port[i].desc,"-");
    }
    
    install_element (VIEW_NODE, &show_interface_cmd);
    install_element (ENABLE_NODE, &show_interface_cmd);
    install_element (CONFIG_NODE, &show_interface_cmd);
    install_element (CONFIG_NODE, &config_one_if_cmd);

    install_element (INTERFACE_NODE, &interface_mtu_cmd);
    install_element (INTERFACE_NODE, &interface_desc_cmd);
    install_element (INTERFACE_NODE, &shutdown_if_cmd);
    install_element (INTERFACE_NODE, &no_shutdown_if_cmd);
    install_element (INTERFACE_NODE, &interface_negotiation_enable_cmd);
    install_element (INTERFACE_NODE, &interface_flow_ctrl_cmd);
    install_element (INTERFACE_NODE, &interface_ethif_speed_cmd);
    install_element (INTERFACE_NODE, &interface_duplex_mode_cmd);
    install_element (INTERFACE_NODE, &set_port_type_cmd);
    install_element (INTERFACE_NODE, &add_access_port_vlan_cmd);
    install_element (INTERFACE_NODE, &config_quit_cmd);
    
}
