TARGET		= libuhttp.so
HEADER		= uhttp.h

CC		= gcc
LD		= ld
CCFLAGS		= -g -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS		= -shared -lrefer -lrbtree -lvattr -lexbuffer -soname ${TARGET}

OBJECT		= uhttp_inst.o uhttp.o uhttp_build.o uhttp_parse.o uhttp_inst_http11.o uhttp_uri.o

%.o: %.c
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
