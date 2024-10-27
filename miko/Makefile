TARGET-LIB	= libmiko.so
TARGET-STD	= libmiko-std.so
TARGET-BIN	= miko
TARGET		= ${TARGET-LIB} ${TARGET-STD} ${TARGET-BIN}
HEADER		= $(wildcard *.h)

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS-LIB	= -shared -lrefer -lhashmap -lrbtree -lvattr -lexbuffer -lmlog -ltparse -lyaw -soname ${TARGET-LIB}
LDFLAGS-STD	= -shared -lm -lrefer -lmlog -L. -lmiko -soname ${TARGET-STD}
LDFLAGS-BIN	= -lrefer -lmlog -lfsys -lyaw -L. -lmiko -lmiko-std

OBJECT-LIB	= $(patsubst %.c, %.o, $(wildcard miko.base/*.c))
OBJECT-STD	= $(patsubst %.c, %.o, $(wildcard miko.std/*.c))
OBJECT-BIN	= $(patsubst %.c, %.o, $(wildcard miko.bin/*.c))

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
	sudo mkdir -p /usr/local/include/miko/
	sudo cp ${HEADER} /usr/local/include/miko/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET-LIB} ${TARGET-STD})
	sudo rm -f $(addprefix /usr/local/bin/, ${TARGET-BIN})
	sudo rm -rf /usr/local/include/miko/
