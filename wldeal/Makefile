TARGET		= wldeal
HEADER		= wldeal.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -Wall -Wextra -Wno-unused-parameter -Wno-multichar
LDFLAGS		= -lrefer -ldylink -lwav -lwavelike -lnote -lscatterplot

OBJECT		= wldeal.o

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

${TARGET}: ${OBJECT}
	${CC} ${LDFLAGS} -o $@ $^

clear:
	rm -f ${TARGET} *.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET} /usr/local/bin/
	sudo cp ${HEADER} /usr/local/include/
uninstall:
	sudo rm -f $(addprefix /usr/local/bin/, ${TARGET})
	sudo rm -f $(addprefix /usr/local/include/, ${HEADER})
