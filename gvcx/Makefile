TARGET		= libgvcx.so
HEADER		= $(wildcard *.h)

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS		= -shared -lrefer -lrbtree -lvattr -soname ${TARGET}

OBJECT		= $(patsubst %.c, %.o, $(wildcard */*.c))

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

${TARGET}: ${OBJECT}
	${LD} ${LDFLAGS} -o $@ $^

clear:
	rm -f ${TARGET} *.o
