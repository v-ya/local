# vya.rg.uniform_n
均匀分布 n 项均值

vya.common.random_generate_f 类型

---
### arg
```
	.n:  integer  [1, 16]  项数
```
---
### detail
```
	rsrc = :
	mu = :
	sigma = :
	N = <.n>
	n = 0
	while (N)
		n += rsrc()
	n /= N
	= (n - 0.5) * sqrt(12 * N) * sigma + mu
```

***
# vya.rg.uniform_n.arg
---
```
	N = <.n>
```

