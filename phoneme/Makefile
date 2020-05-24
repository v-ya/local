TARGET		= libphoneme.so
HEADER		= phoneme_type.h phoneme_arg.h phoneme.h phoneme_pool.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter -Wno-multichar
LDFLAGS		= -shared -lrefer -lhashmap -ljson -ldylink -lnote -Wl,-soname,${TARGET}

OBJECT		= phoneme_type.o phoneme_arg.o phoneme.o phoneme_pool.o

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

${TARGET}: ${OBJECT}
	${CC} ${LDFLAGS} -o $@ $^

clear:
	rm -f ${TARGET} *.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET} /usr/local/lib/
	sudo mkdir -p /usr/local/include/phoneme/
	sudo cp ${HEADER} /usr/local/include/phoneme/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET})
	sudo rm -rf /usr/local/include/phoneme/
