# vya.df.r_an_polyline
随泛波阶数变化带扰动的细节幅值修饰

---
### arg
```
	rgname := vya.rg.normal
	= vya.common.random_polyline
```
---
### detail
```
	rp = <vya.df.r_an_polyline.arg()>
	= sa[n] * exp(vya.common.random_polyline(rp, n))
```

***
# vya.df.r_an_polyline.arg
---
```
	rp = vya.common.random_polyline_arg(<>, rgname = vya.rg.normal, rsname = null)
```
