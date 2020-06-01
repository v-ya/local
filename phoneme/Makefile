TARGET-LIB	= libphoneme.so
TARGET-BIN	= phoneme
TARGET		= ${TARGET-LIB} ${TARGET-BIN}
HEADER		= phoneme_type.h phoneme_buffer.h phoneme_arg.h phoneme_pool.h phoneme.h phoneme_script.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter -Wno-multichar
LDFLAGS-LIB	= -shared -lrefer -lhashmap -ljson -ldylink -lnote -lm -Wl,-soname,${TARGET-LIB}
LDFLAGS-BIN	= -lrefer -lhashmap -largs -ldylink -lwav -lm -L. -lphoneme

OBJECT-LIB	= phoneme_type.o phoneme_buffer.o phoneme_arg.o phoneme.o phoneme_pool.o phoneme_script.o
OBJECT-BIN	= phoneme-entry.o core/core.o

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: all clear install uninstall

all: ${TARGET}

${TARGET-LIB}: ${OBJECT-LIB}
	${CC} ${LDFLAGS-LIB} -o $@ $^

${TARGET-BIN}: ${OBJECT-BIN} ${TARGET-LIB}
	${CC} ${LDFLAGS-BIN} -o $@ ${OBJECT-BIN}

core/core.o:
	+make -C core

clear:
	+make -C core clear
	rm -f ${TARGET} *.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET-LIB} /usr/local/lib/
	sudo cp ${TARGET-BIN} /usr/local/bin/
	sudo mkdir -p /usr/local/include/phoneme/
	sudo cp ${HEADER} /usr/local/include/phoneme/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET-LIB})
	sudo rm -f $(addprefix /usr/local/bin/, ${TARGET-BIN})
	sudo rm -rf /usr/local/include/phoneme/
