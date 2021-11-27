TARGET		= libweb.so
HEADER		= $(wildcard web-*.h)

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS		= -shared -lrefer -lhashmap -lexbuffer -luhttp -ludns -ltransport -lyaw -lqueue -soname ${TARGET}

OBJECT		= $(patsubst %.c, %.o, $(wildcard web-*.c))

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

${TARGET}: ${OBJECT}
	${LD} ${LDFLAGS} -o $@ $^

clear:
	rm -f ${TARGET} *.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET} /usr/local/lib/
	sudo mkdir -p /usr/local/include/web/
	sudo cp ${HEADER} /usr/local/include/web/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET})
	sudo rm -rf /usr/local/include/web/
