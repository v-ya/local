output	= output
list	= $(patsubst %/Makefile,%,$(wildcard */*/Makefile))

%/:
	@mkdir $@

.PHONY: all clear
all: ${output}/
	@$(foreach path, ${list},\
		make --no-print-directory -C ${path}/ &&\
		cp ${path}/$(subst /,.,${path}).kiya ${output}/$(subst /,.,${path});)
clear:
	@rm -f ${output}/*
	@$(foreach path, ${list}, make -s -C $(dir ${path}) clear;)
