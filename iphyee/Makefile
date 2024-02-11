TARGET		= libiphyee.so
HEADER		= iphyee.h iphyee.param.h

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -Wall -Wextra -Wno-unused-parameter
LDFLAGS		= -shared -lm -lrefer -lvattr -lmlog -llayer -lfsys -lvulkan -soname ${TARGET}

OBJECT		= $(patsubst %.c, %.o, $(wildcard *.c) $(wildcard */*.c))

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

%.spv: %.glsl
	glslc -O --target-env=vulkan1.2 -std=450core -fshader-stage=comp -o $@ $<

.PHONY: clear install uninstall

${TARGET}: ${OBJECT}
	${LD} ${LDFLAGS} -o $@ $^

clear:
	rm -f ${TARGET} *.o */*.o glsl/*.spv
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET} /usr/local/lib/
	sudo mkdir -p /usr/local/include/iphyee/
	sudo cp ${HEADER} /usr/local/include/iphyee/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET})
	sudo rm -rf /usr/local/include/iphyee/
