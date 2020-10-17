TARGET-LIB	= librhash.so
TARGET-BIN	= rhash
TARGET		= ${TARGET-LIB} ${TARGET-BIN}
HEADER		= rhash.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS-LIB	= -shared -lrefer -lrandom -soname ${TARGET-LIB}
LDFLAGS-BIN	= -lrefer -lhashmap -largs -L. -lrhash

OBJECT-LIB	= rhash.o
OBJECT-BIN	= rhash-entry.o

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
