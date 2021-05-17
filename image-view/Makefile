TARGET		= image-view

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -Wall -Wextra -Wno-unused-parameter
LDFLAGS		= -lm -lrefer -lhashmap -largs -lmulticalc -lxwindow -lavformat -lavcodec -lswscale -lavutil

ARCH		= $(shell uname -m)
OBJECT		= main.o args.o inst.o image_load.o image_resample.o image_resample_get_dst.o

%.o: %.${ARCH}.S
	${CC} ${CCFLAGS} -c -o $@ $<

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

${TARGET}: ${OBJECT}
	${CC} ${LDFLAGS} -o $@ $^

clear:
	rm -f ${TARGET} *.o
install: ${TARGET}
	sudo cp ${TARGET} /usr/local/bin/
uninstall:
	sudo rm -f $(addprefix /usr/local/bin/, ${TARGET})
