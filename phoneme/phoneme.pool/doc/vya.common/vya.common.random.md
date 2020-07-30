# vya.common.random
随机数产生器

---
### arg
```
	.@:      string | null    随机数产生器名称 (vya.rg.*)
	.@rsrc:  vya.common.rsrc  随机数发生源
	<>:      object           随机数产生器构造参数
```
---
### detail
```
	mu = :
	sigma = :
	rsrc = <vya.common.random_arg()>
	rg_func = <vya.common.random_arg()>
	rg_data = <vya.common.random_arg()>
	= rg_func(rsrc, mu, sigma, rg_data)
```

***
# vya.common.random_arg
---
```
	rsrc = rsrc_alloc(<.@rsrc>)
	rg_func = rgf_find(<.@>)
	if (rg_func) rg_data = vdf_find(<.@>.arg)(<>)
```
