TARGET		= pocket-builder
HEADER		= pocket-builder-kiya.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -Wall -Wextra -Wno-unused-parameter
LDFLAGS		= -ldl -lrefer -lhashmap -lexbuffer -largs -lmlog -ljson -lfsys -lpocket -lpocket-saver -lpocket-verify

OBJECT		= entry.o entry_arg.o pocket_dump.o script.o script_header.o script_ptag.o script_parse.o

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

${TARGET}: ${OBJECT}
	${CC} ${LDFLAGS} -o $@ $^

clear:
	rm -f ${TARGET} *.o
install: ${TARGET}
	sudo cp ${TARGET} /usr/local/bin/
	sudo cp ${HEADER} /usr/local/include/
uninstall:
	sudo rm -f $(addprefix /usr/local/bin/, ${TARGET})
	sudo rm -f $(addprefix /usr/local/include/, ${HEADER})
