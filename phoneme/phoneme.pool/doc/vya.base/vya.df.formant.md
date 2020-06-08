# vya.df.formant
正态分布共振峰修饰

---
### arg
```
	[].u:  floating  R     共振峰中心频率
	[].c:  floating  != 0  共振峰宽度，标准差
	[].a:  floating  R     共振峰峰值增加倍率
```
---
### detail
```
	f = :
	p[].u = <[].u>
	p[].c = <[].c>
	p[].a = <[].a>
	for p in p[]
		for n in =
			nf = n * f
			q = (nf - p->u) / p->c
			k = 1 + p->a * exp(-0.5 * q * q)
			= sa[n] *= k
```

***
# vya.df.formant.arg
---
```
	p[].u = <[].u>
	p[].c = <[].c>
	p[].a = <[].a>
```
