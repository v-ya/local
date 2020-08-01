# vya.bf.random
带扰动的原始基频

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
	rc = <vya.bf.random.arg()>
	= basefre * exp(vya.common.random_const(rc))
```

***
# vya.bf.random.arg
---
```
	rc = vya.common.random_const_arg(<>, rgname = vya.rg.normal, rsname = null)
```
