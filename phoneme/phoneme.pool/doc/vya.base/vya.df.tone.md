# vya.df.tone
音色修饰

---
### arg
```
	.tone:    array[floating]  音色数组
	.length:  floating         tone 范围 (= 20000 Hz)
	.offset:  floating         tone 偏移 (= 0 Hz)
	.g:       floating         tone 积分粒度 (= 10 Hz)
```
---
### detail
```
	pri = <vya.df.tone.arg()>
	basefre = :
	= d->saq[n].sa *= exp(seqarray(pri, basefre * (n + 1)));
```

***
# vya.df.tone.arg
---
```
	tone = <.tone>
	length = <.length>
	offset = <.offset>
	g = <.g>
	integral = 0
	value = []
	for (x = 0; x < length; x += g)
		integral += tone[x]
		value[x] = integral
	= vya.comman.seqarray_alloc(value, value.length, -offset, length)
```
