: verify   = "*>^~64.4"
: tag      = "kiya"
: version  = "kirakira"
: author   = "vya"
[$] "description" @text => "\tC 标准数学库函数"
: desc     = "description"

[$ "kiya" kiya]
	"name" @string => "import.math"
[kiya "parse"]
	"kiya.export" @null
[kiya "dylink" "libm.so.6"]
	"abs" @null
	"labs" @null
	"llabs" @null
	"fabs" @null
	"fmod" @null
	"remainder" @null
	"remquo" @null
	"fma" @null
	"fmax" @null
	"fmin" @null
	"fdim" @null
	"exp" @null
	"exp2" @null
	"expm1" @null
	"log" @null
	"log2" @null
	"log10" @null
	"log1p" @null
	"ilogb" @null
	"logb" @null
	"sqrt" @null
	"cbrt" @null
	"hypot" @null
	"pow" @null
	"sin" @null
	"cos" @null
	"tan" @null
	"asin" @null
	"acos" @null
	"atan" @null
	"atan2" @null
	"sinh" @null
	"cosh" @null
	"tanh" @null
	"asinh" @null
	"acosh" @null
	"atanh" @null
	"ceil" @null
	"floor" @null
	"trunc" @null
	"round" @null
	"lround" @null
	"llround" @null
	"nearbyint" @null
	"rint" @null
	"lrint" @null
	"llrint" @null
	"frexp" @null
	"ldexp" @null
	"modf" @null
	"scalbn" @null
	"scalbln" @null
	"nextafter" @null
	"nexttoward" @null
	"copysign" @null

[] "kiya.export" @"kiya.export.all"
