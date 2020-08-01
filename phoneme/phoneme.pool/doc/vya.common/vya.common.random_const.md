# vya.common.random_const
固定 (mu, sigma) 随机数产生器

---
### arg
```
	.type:   vya.common.random  随机数产生器
	.u:      floating           随机数产生器  均值    mu
	.c:      floating           随机数产生器  标准差  sigma
```
---
### detail
```
	r = <vya.common.random_const_arg()>
	mu = <vya.common.random_const_arg()>
	sigma = <vya.common.random_const_arg()>
	= vya.common.random(r, mu, sigma)
```

***
# vya.common.random_const_arg
---
```
	r = vya.common.random_arg(<.type>)
	mu = <.u>
	sigma = <.c>
```
