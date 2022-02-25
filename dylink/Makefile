TARGET-LIB	= libdylink.so
TARGET-BIN	= dylink
TARGET		= ${TARGET-LIB} ${TARGET-BIN}
HEADER		= dylink.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter -Wno-multichar
LDFLAGS-LIB	= -shared -lhashmap -soname ${TARGET-LIB}
LDFLAGS-BIN	= -lrefer -lhashmap -lexbuffer -lvattr -lfsys -lelfin -L. -ldylink

OBJECT-LIB	= dylink.o xmem.o
OBJECT-BIN	= dylink-entry.o elf2dylink.o

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: all clear install uninstall

all: ${TARGET}

${TARGET-LIB}: ${OBJECT-LIB}
	${LD} ${LDFLAGS-LIB} -o $@ $^

${TARGET-BIN}: ${OBJECT-BIN} ${TARGET-LIB}
	${CC} ${LDFLAGS-BIN} -o $@ ${OBJECT-BIN}

clear:
	rm -f ${TARGET} *.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET-LIB} /usr/local/lib/
	sudo cp ${TARGET-BIN} /usr/local/bin/
	sudo cp ${HEADER} /usr/local/include/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET-LIB})
	sudo rm -f $(addprefix /usr/local/bin/, ${TARGET-BIN})
	sudo rm -f $(addprefix /usr/local/include/, ${HEADER})
