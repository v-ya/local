output	= output
list	= $(patsubst %/Makefile, '%', $(wildcard */*/Makefile))

%/:
	@mkdir $@

.PHONY: all clear sync source remove
all: ${output}/ pocket-builder-kiya/kiiyaa
	@$(foreach path, ${list},\
		make --no-print-directory -C ${path}/ &&\
		cp ${path}/$(subst /,.,${path}).kiya ${output}/$(subst /,.,${path});)
clear:
	@rm -f ${output}/*
	@make --no-print-directory -C pocket-builder-kiya/ clear
	@$(foreach path, ${list}, make --no-print-directory -C ${path}/ clear;)
sync:
	sudo mkdir -p /usr/local/kiiyaa/
	sudo kiya-source-builder /usr/local/kiiyaa/ -s ${output}

source:
	sudo kiya-source-builder /usr/local/kiiyaa/ -o /usr/local/kiiyaa/source

remove:
	sudo rm -rf /usr/local/kiiyaa/

# tool

pocket-builder-kiya/kiiyaa:
	@make --no-print-directory -C pocket-builder-kiya/
