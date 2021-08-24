TARGET		= libweb.so
HEADER		= web-client.h web-server.h web-transport.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS		= -shared -lrefer -lexbuffer -luhttp -ltransport -soname ${TARGET}

OBJECT		= web-client.o web-server.o web-transport.o web-request.o web-method.o

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
