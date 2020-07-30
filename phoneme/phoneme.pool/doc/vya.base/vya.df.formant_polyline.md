# vya.df.formant_polyline
正态分布共振峰偏移修饰

---
### arg
```
	[].u:  vya.common.polyline  共振峰中心频率
	[].c:  vya.common.polyline  共振峰宽度，标准差
	[].a:  vya.common.polyline  共振峰峰值增加倍率
```
---
### detail
```
	t = :
	pl[] = <vya.df.formant_polyline.arg()>
	p[].u = vya.common.polyline(t, <pl[].u>)
	p[].c = vya.common.polyline(t, <pl[].c>)
	p[].a = vya.common.polyline(t, <pl[].a>)
	@see vya.df.formant
```

***
# vya.df.formant_polyline.arg
---
```
	pl[].u = vya.common.polyline_arg(<[].u>, asym = null, bsym = null)
	pl[].c = vya.common.polyline_arg(<[].c>, asym = null, bsym = null)
	pl[].a = vya.common.polyline_arg(<[].a>, asym = null, bsym = null)
```
