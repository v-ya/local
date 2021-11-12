output	= output
list	= $(patsubst %/Makefile,%,$(wildcard */*/Makefile))

%/:
	@mkdir $@

.PHONY: all clear sync source remove
all: ${output}/
	@$(foreach path, ${list},\
		make --no-print-directory -C ${path}/ &&\
		cp ${path}/$(subst /,.,${path}).kiya ${output}/$(subst /,.,${path});)
clear:
	@rm -f ${output}/* import/*.o import/*.dy import/*.kiya
	@$(foreach path, ${list}, make --no-print-directory -C ${path}/ clear;)

sync:
	sudo mkdir -p /usr/local/kiiyaa/
	sudo kiya-source-builder /usr/local/kiiyaa/ -s ${output}

source:
	sudo kiya-source-builder /usr/local/kiiyaa/ -o /usr/local/kiiyaa/source

remove:
	sudo rm -rf /usr/local/kiiyaa/
