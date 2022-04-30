TARGET-LIB	= libvkaa.so
TARGET-STD	= libvkaa-std.so
TARGET-BIN	= vkaa
TARGET		= ${TARGET-LIB} ${TARGET-STD} ${TARGET-BIN}
HEADER		= $(wildcard *.h)

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS-LIB	= -shared -lrefer -lhashmap -lrbtree -lvattr -lexbuffer -ltparse -soname ${TARGET-LIB}
LDFLAGS-STD	= -shared -lrefer -L. -lvkaa -soname ${TARGET-STD}
LDFLAGS-BIN	= -lrefer -lfsys -L. -lvkaa -lvkaa-std

OBJECT-LIB	= $(patsubst %.c, %.o, $(wildcard vkaa.base/*.c))
OBJECT-STD	= $(patsubst %.c, %.o, $(wildcard vkaa.std/*.c))
OBJECT-BIN	= $(patsubst %.c, %.o, $(wildcard vkaa.bin/*.c))

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

all: ${TARGET}

${TARGET-LIB}: ${OBJECT-LIB}
	${LD} ${LDFLAGS-LIB} -o $@ ${OBJECT-LIB}

${TARGET-STD}: ${OBJECT-STD} ${TARGET-LIB}
	${LD} ${LDFLAGS-STD} -o $@ ${OBJECT-STD}

${TARGET-BIN}: ${OBJECT-BIN} ${TARGET-LIB} ${OBJECT-STD}
	${CC} ${LDFLAGS-BIN} -o $@ ${OBJECT-BIN}

clear:
	rm -f ${TARGET} */*.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET-LIB} ${TARGET-STD} /usr/local/lib/
	sudo cp ${TARGET-BIN} /usr/local/bin/
	sudo mkdir -p /usr/local/include/vkaa/
	sudo cp ${HEADER} /usr/local/include/vkaa/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET-LIB} ${TARGET-STD})
	sudo rm -f $(addprefix /usr/local/bin/, ${TARGET-BIN})
	sudo rm -rf /usr/local/include/vkaa/
