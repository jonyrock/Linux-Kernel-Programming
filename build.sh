#!/bin/bash
echo "Building $0/$1"
rm $1.o
rm $1.ko
echo "obj-m += $1.o" > Makefile
make -C ../linux-3.3-rc2/ SUBDIRS=$PWD modules 
rmmod $1.
echo "------- $1 `date` -------" >> /var/log/syslog
insmod $1.ko
rmmod $1.