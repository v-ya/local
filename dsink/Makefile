TARGET		= libdsink.so
HEADER		= dsink.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS		= -lm -shared -Wl,-soname=${TARGET}

ARCH		= $(shell uname -m)
OBJECT		= dsink.o $(patsubst %.c, %.o, $(wildcard dsink/*.c))

%.o: %.${ARCH}.S
	${CC} ${CCFLAGS} -c -o $@ $<

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
