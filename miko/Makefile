TARGET-WINK	= libmiko-wink.so
TARGET-LIB	= libmiko.so
TARGET-STD	= libmiko-std.so
TARGET-BIN	= miko
TARGET		= ${TARGET-WINK} ${TARGET-LIB} ${TARGET-STD} ${TARGET-BIN}
HEADER		= $(wildcard *.h)

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS-WINK	= -shared -lrefer -lexbuffer -lqueue -lyaw -lmlog -soname ${TARGET-WINK}
LDFLAGS-LIB	= -shared -lrefer -lhashmap -lrbtree -lvattr -lexbuffer -lmlog -ltparse -lyaw -L. -lmiko-wink -soname ${TARGET-LIB}
LDFLAGS-STD	= -shared -lm -lrefer -lmlog -L. -lmiko-wink -lmiko -soname ${TARGET-STD}
LDFLAGS-BIN	= -lrefer -lmlog -lfsys -lyaw -L. -lmiko-wink -lmiko -lmiko-std

OBJECT-WINK	= $(patsubst %.c, %.o, $(wildcard miko.wink/*.c))
OBJECT-LIB	= $(patsubst %.c, %.o, $(wildcard miko.base/*.c))
OBJECT-STD	= $(patsubst %.c, %.o, $(wildcard miko.std/*.c))
OBJECT-BIN	= $(patsubst %.c, %.o, $(wildcard miko.bin/*.c))

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

all: ${TARGET}

${TARGET-WINK}: ${OBJECT-WINK}
	${LD} ${LDFLAGS-WINK} -o $@ ${OBJECT-WINK}

${TARGET-LIB}: ${OBJECT-LIB} ${TARGET-WINK}
	${LD} ${LDFLAGS-LIB} -o $@ ${OBJECT-LIB}

${TARGET-STD}: ${OBJECT-STD} ${TARGET-WINK} ${TARGET-LIB}
	${LD} ${LDFLAGS-STD} -o $@ ${OBJECT-STD}

${TARGET-BIN}: ${OBJECT-BIN} ${TARGET-WINK} ${TARGET-LIB} ${OBJECT-STD}
	${CC} ${LDFLAGS-BIN} -o $@ ${OBJECT-BIN}

clear:
	rm -f ${TARGET} */*.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET-WINK} ${TARGET-LIB} ${TARGET-STD} /usr/local/lib/
	sudo cp ${TARGET-BIN} /usr/local/bin/
	sudo mkdir -p /usr/local/include/miko/
	sudo cp ${HEADER} /usr/local/include/miko/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET-WINK} ${TARGET-LIB} ${TARGET-STD})
	sudo rm -f $(addprefix /usr/local/bin/, ${TARGET-BIN})
	sudo rm -rf /usr/local/include/miko/
