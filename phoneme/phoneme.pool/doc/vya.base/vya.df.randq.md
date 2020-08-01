# vya.df.randq
随机数填充所有使用的相位

---
### arg
```
	rgname := vya.rg.normal
	= vya.common.random_const
```
---
### detail
```
	rc = <vya.df.randq.arg()>
	= sq[n] = vya.common.random_const(rc)
```

***
# vya.df.randq.arg
---
```
	rc = vya.common.random_const_arg(<>, rgname = vya.rg.normal, rsname = null)
```
