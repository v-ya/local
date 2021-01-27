project		= refer hashmap rbtree json largeN matrix mlog args dylink random rhash pocket atlayer note wav bmp wavelike pcm2wav pcmplay audioloop scatterplot wldeal phoneme phloop graph iyaa

.PHONY: clear install
clear:
	+for p in ${project}; do\
		make -C $$p $@;\
	done

install:
	+for p in ${project}; do\
		make -C $$p $@;\
	done
