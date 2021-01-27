TARGET-POCKET	= libpocket.so
TARGET-SAVER	= libpocket-saver.so
TARGET-VERIFY	= libpocket-verify.so
TARGET		= ${TARGET-POCKET} ${TARGET-SAVER} ${TARGET-VERIFY}
HEADER		= pocket.h pocket-saver.h pocket-verify.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS-POCKET	= -shared -soname ${TARGET-POCKET} -lrefer -lrbtree
LDFLAGS-SAVER	= -shared -soname ${TARGET-SAVER} -lrefer -lhashmap -lrbtree
LDFLAGS-VERIFY	= -shared -soname ${TARGET-VERIFY} -lrefer -lhashmap

OBJECT-POCKET	= pocket.o
OBJECT-SAVER	= pocket-saver.o
OBJECT-VERIFY	= pocket-verify.o $(patsubst %.c, %.o, $(wildcard verify/*.c))
OBJECT		= ${OBJECT-POCKET} ${OBJECT-SAVER} ${OBJECT-VERIFY}

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

${TARGET}: ${OBJECT}
	${LD} ${LDFLAGS-POCKET} -o ${TARGET-POCKET} ${OBJECT-POCKET}
	${LD} ${LDFLAGS-SAVER} -o ${TARGET-SAVER} ${OBJECT-SAVER}
	${LD} ${LDFLAGS-VERIFY} -o ${TARGET-VERIFY} ${OBJECT-VERIFY}

clear:
	rm -f ${TARGET} *.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET} /usr/local/lib/
	sudo mkdir -p /usr/local/include/pocket/
	sudo cp ${HEADER} /usr/local/include/pocket/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET})
	sudo rm -rf /usr/local/include/pocket/
