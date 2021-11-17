cc		= gcc
dylink		= dylink
ccflags		= -O2 -Wall -Wextra -Wno-unused-parameter -fno-stack-protector
pocket-builder	= pocket-builder

kiya-list	= $(patsubst %.c, %.kiya, $(wildcard *.c))

%.o: %.c
	${cc} ${ccflags} -c $< -o $@

%.dy: %.o
	${dylink} $< -o $@

%.kiya: %.builder %.dy
	${pocket-builder} $< -o $@

kiiyaa: kiiyaa.builder ${kiya-list}
	${pocket-builder} $< -o $@

.PHONY: clear
clear:
	rm -f *.o *.dy *.kiya kiiyaa
