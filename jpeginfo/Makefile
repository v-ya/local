TARGET		= jpeginfo

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -Wall -Wextra -Wno-unused-parameter
LDFLAGS		= -lrefer -lhashmap -lrbtree -lexbuffer -lfsys -lmlog -lyaw -lxwindow -lm

OBJECT		= $(patsubst %.c, %.o, $(wildcard *.c))

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
