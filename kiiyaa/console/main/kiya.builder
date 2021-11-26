: verify   = "*>^~64.4"
: tag      = "kiya"
: author   = "vya"
[$] "description" @text => @"desc.txt"
: desc     = "description"

[$ "kiya"] "name" @string => "console.main"
[$ "kiya" "like"]
	"import.refer" @null
	"import.mlog" @null
	"import.pocket" @null
	"import.dylink" @null
	"import.exbuffer" @null
	"import.hashmap" @null
	"import.vattr" @null

[$ "kiya" "dylink" "libc.so.6"]
	"read" @null
	"poll" @null
	"signal" @null
[$ "kiya" "dylink"]
	"console.main" @"dylink" => &"console.main.dy"

[$ "kiya" "parse"]
	"kiya.export" @null

[$ "kiya"]
	"initial" @string => "initial"
	"finally" @string => "finally"

["kiya.export"]
	"console_main" @string => "main"
