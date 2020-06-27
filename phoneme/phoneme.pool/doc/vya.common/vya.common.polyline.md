# vya.common.polyline
折线

---
### arg
```
	.=[].s:  floating  [0, ...)+  位置
	.=[].v:  floating  [0, 1]     值
```
```
	; 位置等分 [0, 1]
	.=[]:    floating  [0, 1]     值
```
```
	; 可选字段
	.-.@:    string | null  前置处理 (vya.common.value_deal_f 类型)
		.-              前置处理参数
	.+.@:    string | null  后置处理 (vya.common.value_deal_f 类型)
		.+              后置处理参数
```
---
### detail
```
	value = :
	a_func = <vya.common.polyline_arg()>
	a_pri = <vya.common.polyline_arg()>
	b_func = <vya.common.polyline_arg()>
	b_pri = <vya.common.polyline_arg()>
	s[] = <.=[].s>
	v[] = <.=[].v>
	if (a_func) r = seqence(s[], a_func(v[], a_pri), value)
	else r = seqence(s[], v[], value)
	if (b_func) r = b_func(r, b_pri)
	= r
```

***
# vya.common.polyline_arg
---
```
	s[] = <.=[]> is object ? <.=[].s> : [0 ... 1]
	v[] = <.=[]> is object ? <.=[].v> : <.=[]>
	a_func = vdf_find(<.-.@>)
	if (a_func) a_pri = vdf_find(<.-.@>.arg)(<.->)
	b_func = vdf_find(<.+.@>)
	if (b_func) b_pri = vdf_find(<.+.@>.arg)(<.+>)
```
