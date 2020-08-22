TARGET		= libgraph.so
HEADER		= graph.h allocator.h support.h device.h surface.h image.h type.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS		= -shared -lrefer -lhashmap -lmlog -lvulkan -lxcb -Wl,-soname,${TARGET}

OBJECT		= graph.o allocator.o support.o vulkan_debug.o device.o surface.o surface_xcb.o image.o type.o

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

${TARGET}: ${OBJECT}
	${CC} ${LDFLAGS} -o $@ $^

clear:
	rm -f ${TARGET} *.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET} /usr/local/lib/
	sudo mkdir -p /usr/local/include/graph/
	sudo cp ${HEADER} /usr/local/include/graph/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET})
	sudo rm -rf /usr/local/include/graph/
