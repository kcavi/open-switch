# open-switch
the mini switch/router software

root@ubuntu:~# git clone https://github.com/kcavi/open-switch.git
Cloning into 'open-switch'...
root@ubuntu:~/open-switch# make
gcc -o switch vtysh.c vtysh_user.c buffer.c vtysh_main.c log.c vector.c linklist.c vty.c memory.c if.c command.c -I. -g -Wall -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function
root@ubuntu:~/open-switch# ./switch 

mini switch.
Copyright 1986-2020 xxx.

switch> enable 
switch# show interface 
  Interface         State(a/o)  Mode      Descr
  ge1/0/1           up/up       bridge    -
  ge1/0/2           up/up       bridge    -
  ge1/0/3           up/up       bridge    -
  ge1/0/4           up/up       bridge    -
  ge1/0/5           up/up       bridge    -
  ge1/0/6           up/up       bridge    -
  ge1/0/7           up/up       bridge    -
  ge1/0/8           up/up       bridge    -
  ge1/0/9           up/up       bridge    -
  ge1/0/10          up/up       bridge    -
  ge1/0/11          up/up       bridge    -
  ge1/0/12          up/up       bridge    -
switch# config 
switch(config)# interface gigaethernet 1
switch(config-if)# shutdown 
switch(config-if)# quit
switch(config)# quit
switch# quit
switch> quit

root@ubuntu:~/open-switch# 
