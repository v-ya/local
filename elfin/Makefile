TARGET		= libelfin.so
HEADER		= elfin.h elfin.item.h elfin.def.h elfin.64.h elfin.r.x86_64.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS		= -shared -lrefer -lexbuffer -lhashmap -lrbtree -lvattr -lfsys -lmlog -soname ${TARGET}

OBJECT		= $(patsubst %.c, %.o, $(wildcard *.c) $(wildcard item/*.c) $(wildcard arch/*.c)) elfin.item.build.o

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

${TARGET}: ${OBJECT}
	${LD} ${LDFLAGS} -o $@ $^

elfin.item.h, elfin.item.build.c: build.proxy-call.tool inner.item.func.inc
	./$<

build.proxy-call.tool: tool/build.proxy-call.o
	${CC} -lrefer -lexbuffer -lvattr -ltparse -lcparse -lfsys -o $@ $^

clear:
	rm -f ${TARGET} *.o item/*.o arch/*.o tool/*.o *.tool *.build.c elfin.item.h
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET} /usr/local/lib/
	sudo mkdir -p /usr/local/include/elfin/
	sudo cp ${HEADER} /usr/local/include/elfin/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET})
	sudo rm -rf /usr/local/include/elfin/
