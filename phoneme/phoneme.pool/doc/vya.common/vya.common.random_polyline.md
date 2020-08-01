# vya.common.random_polyline
(mu, sigma) 折点变化的随机数产生器

---
### arg
```
	.type:   vya.common.random    随机数产生器
	.u:      vya.common.polyline  随机数产生器  均值    mu
	.c:      vya.common.polyline  随机数产生器  标准差  sigma
```
---
### detail
```
	t = :
	r = <vya.common.random_const_arg()>
	mu = <vya.common.random_const_arg()>
	sigma = <vya.common.random_const_arg()>
	= vya.common.random(r, vya.common.polyline(t, mu), vya.common.polyline(t, sigma))
```

***
# vya.common.random_polyline_arg
---
```
	r = vya.common.random_arg(<.type>)
	mu = vya.common.polyline_arg(<.u>)
	sigma = vya.common.polyline_arg(<.c>)
```
