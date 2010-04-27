EXE_NAME = newns
CROSS_COMPILE_armv7 = /srv/cs523/build/toolchain/cs07q3armel/build/arm-2007q3/bin/arm-none-linux-gnueabi-gcc
COMPILER = ${CROSS_COMPILE_armv7}

all: ${EXE_NAME}

${EXE_NAME}: newns.c	
	${COMPILER} -o newns newns.c

clean:
	rm ${EXE_NAME}
