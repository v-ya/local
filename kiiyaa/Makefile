output	= output
list	= $(wildcard */*/Makefile)

%/:
	@mkdir $@

.PHONY: all clear
all: ${output}/
	@$(foreach path, ${list},\
		n1=$(word 2,$(subst -, ,$(word 1,$(subst /, ,${path})))) &&\
		n2=$(word 2,$(subst -, ,$(word 2,$(subst /, ,${path})))) &&\
		make --no-print-directory -C $(dir ${path}) &&\
		cp $(dir ${path})$$n1.$$n2.kiya ${output}/$$n1.$$n2;)
clear:
	@rm -f ${output}/*
	@$(foreach path, ${list}, make -s -C $(dir ${path}) clear;)
