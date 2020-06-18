# vya.df.fa_q
定能级相位积分反馈修饰

---
### arg
```
	.kf:  floating  R  频率积分反馈系数
	.ka:  floating  R  振幅积分反馈系数
```
---
### detail
```
	t = :
	f = :
	sa[] = :
	sq[] = :
	n = []
	w = 2 * pi * f
	tq = n * w * t + sq[n]
	= sq[n] += - kf * ∫ t dw - ka * ∫ t * tan(tq) d ln(sa[n])
```

***
# vya.df.fa_q.arg
---
```
	kf = <.kf>
	ka = <.ka>
```
