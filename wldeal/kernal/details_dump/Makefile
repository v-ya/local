TARGET		= details_dump.dy details_build.elf formant_test.elf

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

formant_test.elf: formant_test.o
	${CC} -o $@ $^ -lm -lrefer -ljson -lscatterplot

clear:
	rm -f ${TARGET} *.o
