TARGET		= liblargeN.so
HEADER		= largeN.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS		= -shared -soname ${TARGET}

OBJECT		= $(patsubst %.c, %.o, $(notdir $(wildcard none/*.c)))

arch		= $(uname -m)

%.o: ${arch}/%.S
	${CC} ${CCFLAGS} -c -o $@ $<
%.o: ${arch}/%.s
	${CC} ${CCFLAGS} -c -o $@ $<
%.o: none/%.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

${TARGET}: ${OBJECT}
	${LD} ${LDFLAGS} -o $@ $^

clear:
	rm -f ${TARGET} *.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET} /usr/local/lib/
	sudo cp ${HEADER} /usr/local/include/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET})
	sudo rm -f $(addprefix /usr/local/include/, ${HEADER})
