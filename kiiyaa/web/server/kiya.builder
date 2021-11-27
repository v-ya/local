: verify   = "*>^~64.4"
: tag      = "kiya"
: author   = "vya"
[$] "description" @text => @"desc.txt"
: desc     = "description"

[$ "kiya"] "name" @string => "web.server"
[$ "kiya" "like"]
	"import.refer" @null
	"import.mlog" @null
	"import.pocket" @null
	"import.dylink" @null
	"import.exbuffer" @null
	"import.hashmap" @null
	"import.vattr" @null
	"import.transport" @null
	"import.uhttp" @null
	"import.yaw" @null
	"import.web" @null
	"import.fsys" @null
	"action.$mlog.used-locker" @null

[$ "kiya" "dylink"]
	"web.server" @"dylink" => &"web.server.dy"

[$ "kiya" "parse"]
	"kiya.export" @null

[$ "kiya"]
	"initial" @string => "initial"
	"finally" @string => "finally"

["kiya.export"]
	"web$server$get_save" @null
