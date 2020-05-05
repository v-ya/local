TARGET		= libwav.so
HEADER		= wav.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -g -fPIC -Wall -Wextra -Wno-unused-parameter -Wno-multichar
LDFLAGS		= -shared -Wl,-soname,${TARGET}

OBJECT		= wav.o

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

${TARGET}: ${OBJECT}
	${CC} ${LDFLAGS} -o $@ $^

clear:
	rm -f ${TARGET} *.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET} /usr/local/lib/
	sudo cp ${HEADER} /usr/local/include/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET})
	sudo rm -f $(addprefix /usr/local/include/, ${HEADER})
