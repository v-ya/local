# vya.e.weaken
衰减包络

---
### arg
```
	.u:  floating  [0, 1]  线性偏置点
	.p:  floating  R       衰减系数
```
---
### detail
```
	volume = :
	t = :
	u = <.u>
	p = <.p>
	k = vya.e.weaken.arg()
	= volume * k * pow(t, u) * pow(1 - t, 1 - u) * exp(- p * t)
```

***
# vya.e.weaken.arg
---
```
	u = <.u>
	p = <.p>
	;; k
	if (p != 0)
	{
		dt = sqrt((p + 1) * (p + 1) - 4 * p * u);
		x = (p + 1 - dt) / (2 * p);
	}
	else x = u;
	k = 1;
	k = vya.e.weaken(volume = 1, t = x);
```
