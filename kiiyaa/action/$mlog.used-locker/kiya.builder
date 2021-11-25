: verify   = "*>^~64.4"
: tag      = "kiya"
: author   = "vya"
[$] "description" @text => @"desc.txt"
: desc     = "description"

[$ "kiya"] "name" @string => "action.$mlog.used-locker"
[$ "kiya" "like"]
	"import.refer" @null
	"import.mlog" @null
	"import.yaw" @null

[$ "kiya" "dylink"]
	"action" @"dylink" => &"action.$mlog.used-locker.dy"

[$ "kiya"]
	"initial" @string => "action"
