TARGET-LIB	= libdylink.so
TARGET-BIN	= dylink
TARGET		= ${TARGET-LIB} ${TARGET-BIN}
HEADER		= dylink.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -g -fPIC -Wall -Wextra -Wno-unused-parameter -Wno-multichar
LDFLAGS-LIB	= -shared -lc -Wl,-soname,${TARGET-LIB}
LDFLAGS-BIN	= -lc -lhashmap -lrbtree -L. -ldylink

OBJECT-LIB	= dylink.o
OBJECT-BIN	= dylink-entry.o elfobj.o

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: all clear install uninstall

all: ${TARGET}

${TARGET-LIB}: ${OBJECT-LIB}
	${CC} ${LDFLAGS-LIB} -o $@ $^

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
