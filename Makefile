project		= hashmap rbtree json

.PHONY: clear
clear:
	+for p in ${project}; do\
		make -C $$p clear;\
	done
