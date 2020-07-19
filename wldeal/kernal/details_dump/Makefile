TARGET		= details_dump.dy details_build

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -Wall -Wextra -Wno-unused-parameter -fno-stack-protector

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: all clear

all: ${TARGET}

details_dump.dy: details_dump.o
	dylink -o $@ $<

details_build: details_build.o
	${CC} -o $@ $^ -lrefer -ljson -lwav -lnote

clear:
	rm -f ${TARGET} *.o
