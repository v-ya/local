# vya.bf.polyline
折线基频

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
	basefre = :
	t = :
	s[] = <[].s>
	v[] = <[].v>
	= basefre * seqence(s[], v[], t)
```

***
# vya.bf.polyline.arg
---
```
	s[] = <[].s>
	v[] = <[].v>
```
```
	s[] = [0 ... 1]
	v[] = <[]>
```
