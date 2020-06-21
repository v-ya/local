project		= refer hashmap rbtree json mlog args dylink note wav bmp wavelike pcm2wav pcmplay audioloop scatterplot wldeal phoneme phloop

.PHONY: clear install
clear:
	+for p in ${project}; do\
		make -C $$p $@;\
	done

install:
	+for p in ${project}; do\
		make -C $$p $@;\
	done
