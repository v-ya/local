TARGET		= libelfin.so
HEADER		= elfin.h elfin.def.h elfin.64.h elfin.r.x86_64.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS		= -shared -lrefer -lexbuffer -lrbtree -lvattr -lfsys -soname ${TARGET}

OBJECT		= $(patsubst %.c, %.o, $(wildcard *.c))

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

${TARGET}: ${OBJECT}
	${LD} ${LDFLAGS} -o $@ $^

clear:
	rm -f ${TARGET} *.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET} /usr/local/lib/
	sudo mkdir -p /usr/local/include/elfin/
	sudo cp ${HEADER} /usr/local/include/elfin/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET})
	sudo rm -rf /usr/local/include/elfin/
