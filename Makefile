project		= refer hashmap rbtree json matrix mlog args dylink random rhash note wav bmp wavelike pcm2wav pcmplay audioloop scatterplot wldeal phoneme phloop graph

.PHONY: clear install
clear:
	+for p in ${project}; do\
		make -C $$p $@;\
	done

install:
	+for p in ${project}; do\
		make -C $$p $@;\
	done
