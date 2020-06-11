# vya.df.vfix
等比例缩放细节使生成的细节段最大值趋进于 1

---
### detail
```
	n = d->used * 8
	x = (max abs {float[n] = detail()}).pos
	(x -= detail'(x) / detail''(x)) loop
	k = 1.0 / detail(x)
	= sa[n] *= k
```
