TARGET		= libatlayer.so
HEADER		= at_layer.h at_cache.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS		= -shared -soname ${TARGET}

OBJECT		= at_layer.o at_cache.o $(patsubst %.c, %.o, $(wildcard imple/*.c))

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

${TARGET}: ${OBJECT}
	${LD} ${LDFLAGS} -o $@ $^

clear:
	rm -f ${TARGET} *.o imple/*.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET} /usr/local/lib/
	sudo mkdir -p /usr/local/include/atlayer/imple/
	sudo cp ${HEADER} /usr/local/include/atlayer/
	sudo cp $(wildcard imple/*.h) /usr/local/include/atlayer/imple/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET})
	sudo rm -rf /usr/local/include/atlayer/
