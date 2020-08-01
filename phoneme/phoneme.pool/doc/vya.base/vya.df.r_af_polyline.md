# vya.df.r_af_polyline
随泛波频率变化带扰动的细节幅值修饰

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
	rp = <vya.df.r_af_polyline.arg()>
	= sa[n] * exp(vya.common.random_polyline(rp, (n + 1) * basefre))
```

***
# vya.df.r_af_polyline.arg
---
```
	rp = vya.common.random_polyline_arg(<>, rgname = vya.rg.normal, rsname = null)
```
