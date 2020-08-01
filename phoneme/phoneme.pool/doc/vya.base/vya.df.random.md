# vya.df.random
带扰动的细节幅值修饰

---
### arg
```
	rgname := vya.rg.normal
	= vya.common.random_const
```
---
### detail
```
	rc = <vya.df.random.arg()>
	= sa[n] * exp(vya.common.random_const(rc))
```

***
# vya.df.random.arg
---
```
	rc = vya.common.random_const_arg(<>, rgname = vya.rg.normal, rsname = null)
```
