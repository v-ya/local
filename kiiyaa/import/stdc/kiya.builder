: verify   = "*>^~64.4"
: tag      = "kiya"
: version  = "kirakira"
: author   = "vya"
[$] "description" @text => "\tC 标准库函数"
: desc     = "description"

[$ "kiya" kiya]
	"name" @string => "import.stdc"
[kiya "parse"]
	"kiya.export" @null
[kiya "dylink" "libc.so.6"]
	# alloc already import by kiya
	# memory
	"memset" @null
	"memcpy" @null
	"memmove" @null
	"memcmp" @null
	"memchr" @null
	# string
	"strcpy" @null
	"strncpy" @null
	"strcat" @null
	"strncat" @null
	"strxfrm" @null
	"strlen" @null
	"strcmp" @null
	"strncmp" @null
	"strcoll" @null
	"strchr" @null
	"strrchr" @null
	"strspn" @null
	"strcspn" @null
	"strpbrk" @null
	"strstr" @null
	"strtok" @null
	# stdlib string => number
	"strtof" @null
	"strtod" @null
	"strtold" @null
	"strtol" @null
	"strtoll" @null
	"strtoul" @null
	"strtoull" @null

[] "kiya.export" @"kiya.export.all"
