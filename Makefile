project		= hashmap rbtree json dylink note wav wavelike

.PHONY: clear
clear:
	+for p in ${project}; do\
		make -C $$p clear;\
	done
