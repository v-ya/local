TARGET		= core.o

CC		= gcc
LD		= ld
DYLINK		= dylink
CCFLAGS		= -O2 -Wall -Wextra -Wno-unused-parameter -fno-stack-protector

OBJECT		= refer.o hashmap.o rbtree.o json.o

%.o:
	${CC} ${CCFLAGS} -c -o $@ $<

%.r.o:
	${LD} -r -o $@ $^

%.dy: %.r.o
	${DYLINK} -o $@ $<

%.o: %.s
	${CC} -c -o $@ $<

.PHONY: clear

${TARGET}: core.s core.dy

core.r.o: ${OBJECT}
refer.o: ../../refer/refer.c
hashmap.o: ../../hashmap/hashmap.c
rbtree.o: ../../rbtree/rbtree.c
json.o: ../../json/json.c

clear:
	rm -f *.o *.dy
