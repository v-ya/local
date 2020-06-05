CC		= gcc
LD		= ld
DYLINK		= dylink
CCFLAGS		= -O2 -Wall -Wextra -Wno-unused-parameter -fno-stack-protector

core		= vya.common vya.base

.PRECIOUS: %.o src/%.r.o

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

ifdef depend
src/%.r.o: ${depend}
	${LD} -r -o $@ $^
else
src/%.r.o: src/%
	+make $@ depend="$(patsubst %.c, %.o, $(wildcard $</*.c))"
endif

core/%: src/%.r.o
	${DYLINK} -o $@ $<

.PHONY: all clear clear-all

all: $(addprefix core/, ${core})

clear:
	rm -f src/*.r.o src/*/*.o

clear-all:
	rm -f core/* src/*.r.o src/*/*.o
