TARGET		= libcmath.so
HEADER		= $(wildcard *.h)

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS		= -shared -lrefer -lexbuffer -soname ${TARGET}

ARCH		= $(shell uname -m)
OBJECT		= $(patsubst %.std.c, %.o, $(wildcard */*.std.c))

%.o: %.${ARCH}.S
	${CC} ${CCFLAGS} -c -o $@ $<

%.o: %.${ARCH}.c
	${CC} ${CCFLAGS} -c -o $@ $<

%.o: %.std.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

${TARGET}: ${OBJECT}
	${LD} ${LDFLAGS} -o $@ $^

clear:
	rm -f ${TARGET} */*.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET} /usr/local/lib/
	sudo mkdir -p /usr/local/include/cmath/
	sudo cp ${HEADER} /usr/local/include/cmath/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET})
	sudo rm -rf /usr/local/include/cmath/
