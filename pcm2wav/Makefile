TARGET		= pcm2wav

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -Wall -Wextra -Wno-unused-parameter -Wno-multichar
LDFLAGS		= -lhashmap -largs -lwav

OBJECT		= pcm2wav.o

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

${TARGET}: ${OBJECT}
	${CC} ${LDFLAGS} -o $@ $^

clear:
	rm -f ${TARGET} *.o
install: ${TARGET}
	sudo cp ${TARGET} /usr/local/bin/
uninstall:
	sudo rm -f $(addprefix /usr/local/bin/, ${TARGET})
