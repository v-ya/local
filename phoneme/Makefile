TARGET-LIB	= libphoneme.so
TARGET-RES	= libphonemeres.so
TARGET-BIN	= phoneme
TARGET		= ${TARGET-LIB} ${TARGET-RES} ${TARGET-BIN}
HEADER		= phoneme_type.h phoneme_output.h phoneme_arg.h phoneme_pool.h phoneme.h phoneme_script.h phoneme_resource.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter -Wno-multichar
LDFLAGS-LIB	= -shared -lm -lpthread -lrefer -lhashmap -ljson -lmlog -ldylink -lnote -soname ${TARGET-LIB}
LDFLAGS-RES	= -shared -lm -lrefer -ldylink -L. -lphoneme -soname ${TARGET-RES}
LDFLAGS-BIN	= -lrefer -lhashmap -lmlog -largs -lwav -L. -lphoneme -lphonemeres

OBJECT-LIB	= phoneme_type.o phoneme_output.o phoneme_arg.o phoneme_pool.o phoneme.o phoneme_script.o
OBJECT-RES	= phoneme_resource.o core/core.o
OBJECT-BIN	= phoneme-entry.o phoneme-entry-arg.o

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: all clear install uninstall

all: ${TARGET}

${TARGET-LIB}: ${OBJECT-LIB}
	${LD} ${LDFLAGS-LIB} -o $@ ${OBJECT-LIB}

${TARGET-RES}: ${OBJECT-RES} ${TARGET-LIB}
	${LD} ${LDFLAGS-RES} -o $@ ${OBJECT-RES}

${TARGET-BIN}: ${OBJECT-BIN} ${TARGET-LIB} ${TARGET-RES}
	${CC} ${LDFLAGS-BIN} -o $@ ${OBJECT-BIN}

core/core.o:
	+make -C core

clear:
	+make -C core clear
	rm -f ${TARGET} *.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET-LIB} ${TARGET-RES} /usr/local/lib/
	sudo cp ${TARGET-BIN} /usr/local/bin/
	sudo mkdir -p /usr/local/include/phoneme/
	sudo cp ${HEADER} /usr/local/include/phoneme/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET-LIB} ${TARGET-RES})
	sudo rm -f $(addprefix /usr/local/bin/, ${TARGET-BIN})
	sudo rm -rf /usr/local/include/phoneme/
