: verify   = "*>^~64.4"
: tag      = "kiya"
: author   = "vya"
[$] "description" @text => @"desc.txt"
: desc     = "description"

[$ "kiya" kiya] "name" @string => "xwin.pool"
[kiya "like"]
	"import.refer" @null
	"import.pocket" @null
	"import.dylink" @null
	"import.hashmap" @null
	"import.vattr" @null
	"import.yaw" @null
	"import.xwindow" @null

[kiya "dylink"]
	"xwin.pool" @"dylink" => &"xwin.pool.dy"

[kiya]
	"initial" @string => "initial"
	"finally" @string => "finally"
