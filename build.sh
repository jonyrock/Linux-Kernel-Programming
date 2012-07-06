#!/bin/bash

echo "Building $0/$1"
rmmod $1
if [ -f $1.o ] 
then 
    rm $1.o 
fi
if [ -f $1.ko ] 
then 
    rm $1.ko 
fi
echo "obj-m += $1.o" > Makefile
make -C ../git_lx/linux/ SUBDIRS=$PWD modules 
rmmod $1
echo "------- $1 `date` -------" >> /var/log/syslog
insmod $1.ko
#sudo mount -t myfs none /mnt/myfs