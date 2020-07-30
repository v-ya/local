# vya.dg.rand
随机数填充所有幅值和相位

---
### arg
```
	rgname := vya.rg.normal
	.a:  vya.common.random_const  幅值分布
	.q:  vya.common.random_const  相位分布
```
---
### detail
```
	rca = <vya.dg.grapow.arg()>
	rcq = <vya.dg.grapow.arg()>
	= sa[n] = (f = vya.common.random_const(rc))>0?f:0
	= sq[n] = vya.common.random_const(rc)
```

***
# vya.df.randa.arg
---
```
	rca = vya.common.random_const_arg(<.a>, rgname = vya.rg.normal, rsname = null)
	rca = vya.common.random_const_arg(<.q>, rgname = vya.rg.normal, rsname = null)
```
