TARGET		= libchord.so
HEADER		= chord.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS		= -shared -soname ${TARGET}

ARCH		= $(shell uname -m)
OBJECT		= chord.o $(patsubst %.c, %.o, $(wildcard zzz/*.c))
ARCH_OBJECT	= $(addprefix zzz/arch/, $(addsuffix .o, $(basename $(notdir $(wildcard zzz/arch/${ARCH}/*)))))

zzz/arch/%.o: zzz/arch/${ARCH}/%.S
	${CC} ${CCFLAGS} -c -o $@ $<

zzz/arch/%.o: zzz/arch/${ARCH}/%.c
	${CC} ${CCFLAGS} -c -o $@ $<

zzz/%.o: zzz/arch/${ARCH}/%.c
	${CC} ${CCFLAGS} -c -o $@ zzz/arch/nop.S

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

${TARGET}: ${OBJECT} ${ARCH_OBJECT}
	${LD} ${LDFLAGS} -o $@ $^

clear:
	rm -f ${TARGET} *.o zzz/*.o zzz/arch/*.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET} /usr/local/lib/
	sudo cp ${HEADER} /usr/local/include/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET})
	sudo rm -f $(addprefix /usr/local/include/, ${HEADER})
