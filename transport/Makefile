TARGET		= libtransport.so
HEADER		= transport.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS		= -shared -lrefer -soname ${TARGET}

OBJECT		= transport.o tcp.o $(patsubst %.c, %.o, $(wildcard inner/*.c))

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

${TARGET}: ${OBJECT}
	${LD} ${LDFLAGS} -o $@ $^

clear:
	rm -f ${TARGET} *.o inner/*.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET} /usr/local/lib/
	sudo mkdir -p /usr/local/include/transport/
	sudo cp ${HEADER} /usr/local/include/transport/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET})
	sudo rm -rf /usr/local/include/transport/
