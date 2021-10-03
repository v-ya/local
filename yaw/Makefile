TARGET		= libyaw.so
HEADER		= yaw.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS		= -shared -lrefer -lpthread -soname ${TARGET}

OBJECT		= yaw.o mutex.o spin.o rwlock.o signal.o

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: verify clear install uninstall

${TARGET}: ${OBJECT}
	${LD} ${LDFLAGS} -o $@ $^

test.elf: _test.o ${TARGET}
	${CC} -lrefer -lpthread -L. -lyaw -Wl,--rpath=. -o $@ $<

verify: test.elf
	valgrind --leak-check=full ./test.elf

clear:
	rm -f ${TARGET} *.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET} /usr/local/lib/
	sudo cp ${HEADER} /usr/local/include/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET})
	sudo rm -f $(addprefix /usr/local/include/, ${HEADER})
