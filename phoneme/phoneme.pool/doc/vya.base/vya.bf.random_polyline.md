# vya.bf.random_polyline
带随时间变化扰动的原始基频

---
### arg
```
	rgname := vya.rg.normal
	= vya.common.random_polyline
```
---
### detail
```
	basefre = :
	t = :
	rp = <vya.bf.random_polyline.arg()>
	= basefre * exp(vya.common.random_polyline(rp, t))
```

***
# vya.bf.random_polyline.arg
---
```
	rp = vya.common.random_polyline_arg(<>, rgname = vya.rg.normal, rsname = null)
```
