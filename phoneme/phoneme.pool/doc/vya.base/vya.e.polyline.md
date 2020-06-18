# vya.e.polyline
折线包络

---
### arg
```
	[].s:  floating  [0, ...)+  位置
	[].v:  floating  [0, 1]     值
```
```
	; 位置等分 [0, 1]
	[]:    floating  [0, 1]     值
```
---
### detail
```
	volume = :
	t = :
	s[] = <[].s>
	v[] = <[].v>
	= volume * seqence(s[], v[], t)
```

***
# vya.e.polyline.arg
---
```
	s[] = <[].s>
	v[] = <[].v>
```
```
	s[] = [0 ... 1]
	v[] = <[]>
```
