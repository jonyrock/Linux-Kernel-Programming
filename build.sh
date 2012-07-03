#!/bin/bash

echo "Building $0/$1"
rmmod $1.ko
if [ -f $1.o ] 
then 
    rm $1.o 
fi
if [ -f $1.ko ] 
then 
    rm $1.ko 
fi
echo "obj-m += $1.o" > Makefile
make -C ../linux-3.3-rc2/ SUBDIRS=$PWD modules 
rmmod $1.ko
echo "------- $1 `date` -------" >> /var/log/syslog
insmod $1.ko