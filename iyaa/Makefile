TARGET		= iyaa

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -Wall -Wextra -Wno-unused-parameter
LDFLAGS		= -lrefer -lmlog -lgraph

OBJECT		= iyaa.o iyii/iyii.o iyii/iyii_graph.o iyii/iyii_swapchain.o iyii/iyii_render.o iyii/iyii_pipeline.o
SHADER		= $(addprefix iyii/shader/, iyii.vert.spv iyii.frag.spv)

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

iyii/shader/%.spv: iyii/shader/%.glsl
	glslangValidator -V -o $@ $<

.PHONY: clear install uninstall

${TARGET}: ${OBJECT} iyii/shader/shader.o
	${CC} ${LDFLAGS} -o $@ $^

iyii/shader/shader.o: iyii/shader/shader.S ${SHADER}
	${CC} ${CCFLAGS} -c -o $@ $<

clear:
	rm -f ${TARGET} *.o iyii/*.o iyii/shader/*.spv iyii/shader/shader.o
install: ${TARGET}
	sudo cp ${TARGET} /usr/local/bin/
uninstall:
	sudo rm -f $(addprefix /usr/local/bin/, ${TARGET})
