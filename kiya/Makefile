TARGET-LIB	= libkiya.so
TARGET-BIN	= kiya
TARGET		= ${TARGET-LIB} ${TARGET-BIN}
HEADER		= kiya.h

CC		= gcc
LD		= ld
DYLINK		= dylink
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
CCFLAGS-DYLINK	= -O2 -Wall -Wextra -Wno-unused-parameter -fno-stack-protector
LDFLAGS-LIB	= -shared -ldl -lrefer -lhashmap -lmlog -ldylink -lpocket -soname ${TARGET-LIB}
LDFLAGS-BIN	= -lrefer -lmlog -lpocket -lpocket-verify -L. -lkiya

OBJECT-LIB	= kiya.o kiya_args.o core/core.o
OBJECT-BIN	= kiya-entry.o

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: all clear install uninstall

all: ${TARGET}

${TARGET-LIB}: ${OBJECT-LIB}
	${LD} ${LDFLAGS-LIB} -o $@ ${OBJECT-LIB}

${TARGET-BIN}: ${OBJECT-BIN} ${TARGET-LIB}
	${CC} ${LDFLAGS-BIN} -o $@ ${OBJECT-BIN}

core/core.o: core/core.s core/core.dy
	${CC} -c -o $@ $<

core/core.dy: core/kiya.r.o
	${DYLINK} -o $@ $<

core/kiya.%.o: core/kiya.%.c
	${CC} ${CCFLAGS-DYLINK} -c -o $@ $<

core/kiya.r.o: core/kiya.var.o core/kiya.load.o core/kiya.export.o
	${LD} -r -o $@ $^

clear:
	rm -f ${TARGET} *.o core/*.o core/*.dy
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET-LIB} /usr/local/lib/
	sudo cp ${TARGET-BIN} /usr/local/bin/
	sudo cp ${HEADER} /usr/local/include/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET-LIB})
	sudo rm -f $(addprefix /usr/local/bin/, ${TARGET-BIN})
	sudo rm -f $(addprefix /usr/local/include/, ${HEADER})
