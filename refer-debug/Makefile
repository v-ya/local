TARGET		= librefer.debug.so
WRAP-MALLOC	= --wrap=malloc --wrap=calloc --wrap=posix_memalign --wrap=aligned_alloc --wrap=memalign --wrap=valloc --wrap=pvalloc --wrap=realloc --wrap=free
WRAP-REFER	= --wrap=refer_alloc --wrap=refer_alloz --wrap=refer_free --wrap=refer_set_free --wrap=refer_get_free --wrap=refer_save --wrap=refer_save_number

CC		= gcc
LD		= ld
CCFLAGS		= -O2 -fPIC -fvisibility=hidden -Wall -Wextra -Wno-unused-parameter
LDFLAGS		= -ldl -lpthread -shared -soname ${TARGET} ${WRAP-MALLOC} ${WRAP-REFER}

OBJECT		= refer.debug.o refer.pool.o inner/rbtree.o inner/queue_ring.o

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

.PHONY: clear install uninstall

${TARGET}: ${OBJECT}
	${LD} ${LDFLAGS} -o $@ $^

clear:
	rm -f ${TARGET} *.o inner/*.o
install: ${TARGET} ${HEADER}
	sudo cp ${TARGET} /usr/local/lib/
uninstall:
	sudo rm -f $(addprefix /usr/local/lib/, ${TARGET})
