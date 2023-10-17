TARGET-ADORA		= libabc-adora.so
TARGET-BLINK		= libabc-blink.so
TARGET-CUTIE		= libabc-cutie.so
TARGET-Z-X86_64		= libabc-z-x86_64.so
TARGET			= ${TARGET-ADORA} ${TARGET-Z-X86_64}
HEADER			= adora.h z.x86_64.h

CC			= gcc
LD			= ld
CCFLAGS			= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS-ADORA		= -shared -soname ${TARGET-ADORA} -lrefer -lexbuffer -lrbtree -lhashmap -lvattr
LDFLAGS-BLINK		= -shared -soname ${TARGET-BLINK} -lrefer
LDFLAGS-CUTIE		= -shared -soname ${TARGET-CUTIE} -lrefer
LDFLAGS-Z-X86_64	= -shared -soname ${TARGET-Z-X86_64}

OBJECT-ADORA		= $(patsubst %.c, %.o, $(wildcard adora/*.c))
OBJECT-BLINK		= 
OBJECT-CUTIE		= 
OBJECT-Z-X86_64		= $(patsubst %.c, %.o, $(wildcard z.x86_64/*.c))
OBJECT			= ${OBJECT-ADORA} ${OBJECT-BLINK} ${OBJECT-CUTIE} ${OBJECT-Z-X86_64}

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

${TARGET}: ${OBJECT}
	${LD} ${LDFLAGS-ADORA} -o ${TARGET-ADORA} ${OBJECT-ADORA}
#	${LD} ${LDFLAGS-BLINK} -o ${TARGET-BLINK} ${OBJECT-BLINK}
#	${LD} ${LDFLAGS-CUTIE} -o ${TARGET-CUTIE} ${OBJECT-CUTIE}
	${LD} ${LDFLAGS-Z-X86_64} -o ${TARGET-Z-X86_64} ${OBJECT-Z-X86_64}

clear:
	rm -f ${TARGET} *.o verify/*.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET} /usr/local/lib/
	sudo mkdir -p /usr/local/include/abc/
	sudo cp ${HEADER} /usr/local/include/abc/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET})
	sudo rm -rf /usr/local/include/abc/
