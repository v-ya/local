# vya.bf.crand
原始基频+随机波动

---
### arg
```
	rgname := vya.rg.normal
	= vya.common.random_const
```
---
### detail
```
	basefre = :
	rc = <vya.bf.crand.arg()>
	k = 1 + vya.common.random_const(rc)
	if (k < 0) k = 0
	= basefre * k
```

***
# vya.bf.crand.arg
---
```
	rc = vya.common.random_const_arg(<>, rgname = vya.rg.normal, rsname = null)
```
