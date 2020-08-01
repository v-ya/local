# vya.df.r_at_polyline
随时间变化带扰动的细节幅值修饰

---
### arg
```
	rgname := vya.rg.normal
	= vya.common.random_polyline
```
---
### detail
```
	t = :
	rp = <vya.df.r_at_polyline.arg()>
	= sa[n] * exp(vya.common.random_polyline(rp, t))
```

***
# vya.df.r_at_polyline.arg
---
```
	rp = vya.common.random_polyline_arg(<>, rgname = vya.rg.normal, rsname = null)
```
