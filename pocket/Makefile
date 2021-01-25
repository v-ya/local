TARGET-POCKET	= libpocket.so
TARGET-SAVER	= libpocket-saver.so
TARGET		= ${TARGET-POCKET} ${TARGET-SAVER}
HEADER		= pocket.h pocket-saver.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS-POCKET	= -shared -soname ${TARGET-POCKET} -lrefer -lrbtree
LDFLAGS-SAVER	= -shared -soname ${TARGET-SAVER} -lrefer -lhashmap -lrbtree

OBJECT-POCKET	= pocket.o
OBJECT-SAVER	= pocket-saver.o
OBJECT		= ${OBJECT-POCKET} ${OBJECT-SAVER}

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

${TARGET}: ${OBJECT}
	${LD} ${LDFLAGS-POCKET} -o ${TARGET-POCKET} ${OBJECT-POCKET}
	${LD} ${LDFLAGS-SAVER} -o ${TARGET-SAVER} ${OBJECT-SAVER}

clear:
	rm -f ${TARGET} *.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET} /usr/local/lib/
	sudo cp ${HEADER} /usr/local/include/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET})
	sudo rm -f $(addprefix /usr/local/include/, ${HEADER})
