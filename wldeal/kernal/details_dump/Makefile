TARGET		= details_dump.dy details_build.elf tone_test.elf tone_cross.elf

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -Wall -Wextra -Wno-unused-parameter -fno-stack-protector

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: all clear

all: ${TARGET}

details_dump.dy: details_dump.o
	dylink -o $@ $<

details_build.elf: details_build.o
	${CC} -o $@ $^ -lrefer -ljson -lwav -lnote

tone_test.elf: tone_test.o
	${CC} -o $@ $^ -lm -lrefer -ljson -lscatterplot

tone_cross.elf: tone_cross.o
	${CC} -o $@ $^ -lm -lrefer -ljson -lscatterplot

clear:
	rm -f ${TARGET} *.o
