#!/bin/bash
#Little script to build qemu the way I'm using it.
#Assumes it's run in a sb2 env

LDFLAGS="-Wl,-rpath=/usr/local/lib -L/usr/local/lib -lz" ./configure \
--prefix= \
--enable-sdl \
--audio-drv-list=sdl \
--disable-system \
--target-list=arm-linux-user,i386-linux-user,x86_64-linux-user,arm-softmmu,i386-softmmu,x86_64-softmmu && \
\
make -j8 && \

make DESTDIR=`pwd`/../install install


