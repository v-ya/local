# vya.df.qfix
等比例缩放各等级相位使起始点为 0

---
### detail
```
	detail_zero(x) = sum sa[n] * sin(sq[n] * x)
	k = 1
	(k -= detail_zero(k) / detail_zero'(k)) loop
	= sq[n] *= k
```
