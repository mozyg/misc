LDFLAGS="-Wl,-rpath=/usr/local/lib -L/usr/local/lib -lz" ./configure --prefix=/usr/local \
--disable-sdl \
--target-list=arm-softmmu,i386-softmmu,x86_64-softmmu \
--disable-system \
--disable-kvm \
--enable-curses \
--audio-drv-list= \
--extra-cflags="-I/usr/local/include -I/usr/local/include/ncurses"
