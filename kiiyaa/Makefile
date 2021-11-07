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
	@rm -f ${output}/* import/*.o import/*.dy import/*.kiya
	@$(foreach path, ${list}, make --no-print-directory -C ${path}/ clear;)
