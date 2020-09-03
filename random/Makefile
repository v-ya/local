TARGET-RANDOM	= librandom.so
TARGET-CHECK	= librandomcheck.so
TARGET		= ${TARGET-RANDOM} ${TARGET-CHECK}
HEADER		= random.h random_check.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS-RANDOM	= -shared -lrefer -ldylink -soname ${TARGET-RANDOM}
LDFLAGS-CHECK	= -shared -lm -lrefer -lmlog -soname ${TARGET-CHECK}

OBJECT-RANDOM	= random_sequence.o
OBJECT-CHECK	= random_check.o random_check_bits.o random_check_mod.o

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: all clear install uninstall

all: ${TARGET}

${TARGET-RANDOM}: ${OBJECT-RANDOM}
	${LD} ${LDFLAGS-RANDOM} -o $@ ${OBJECT-RANDOM}

${TARGET-CHECK}: ${OBJECT-CHECK}
	${LD} ${LDFLAGS-CHECK} -o $@ ${OBJECT-CHECK}

clear:
	rm -f ${TARGET} *.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET-RANDOM} ${TARGET-CHECK} /usr/local/lib/
	sudo mkdir -p /usr/local/include/random/
	sudo cp ${HEADER} /usr/local/include/random/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET-RANDOM} ${TARGET-CHECK})
	sudo rm -rf /usr/local/include/random/
