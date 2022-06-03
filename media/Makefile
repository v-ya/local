TARGET	= libmedia.so
HEADER	= $(wildcard *.h)

CC	= gcc
LD	= ld
CCFLAGS	= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS	= -shared -lrefer -lhashmap -lexbuffer -lfsys -soname ${TARGET}

OBJECT	= media.o $(patsubst %.c, %.o, $(wildcard */*.c))

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

all: ${TARGET}

${TARGET}: ${OBJECT}
	${LD} ${LDFLAGS} -o $@ ${OBJECT}

clear:
	rm -f ${TARGET} *.o */*.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET} /usr/local/lib/
	sudo mkdir -p /usr/local/include/media/
	sudo cp ${HEADER} /usr/local/include/media/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET})
	sudo rm -rf /usr/local/include/media/
