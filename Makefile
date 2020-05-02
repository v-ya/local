project		= hashmap rbtree json dylink

.PHONY: clear
clear:
	+for p in ${project}; do\
		make -C $$p clear;\
	done
