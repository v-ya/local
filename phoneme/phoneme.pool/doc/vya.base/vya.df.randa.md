# vya.df.randa
随机数填充所有使用的幅值

---
### arg
```
	rgname := vya.rg.normal
	= vya.common.random_const
```
---
### detail
```
	rc = <vya.df.randa.arg()>
	= sa[n] = (f = vya.common.random_const(rc))>0?f:0
```

***
# vya.df.randa.arg
---
```
	rc = vya.common.random_const_arg(<>, rgname = vya.rg.normal, rsname = null)
```
