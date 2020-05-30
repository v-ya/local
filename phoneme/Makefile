TARGET		= libphoneme.so
HEADER		= phoneme_type.h phoneme_buffer.h phoneme_arg.h phoneme_pool.h phoneme.h phoneme_script.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter -Wno-multichar
LDFLAGS		= -shared -lrefer -lhashmap -ljson -ldylink -lnote -lm -Wl,-soname,${TARGET}

OBJECT		= phoneme_type.o phoneme_buffer.o phoneme_arg.o phoneme.o phoneme_pool.o phoneme_script.o

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
