: verify   = "*>^~64.4"
: tag      = "kiya"
: author   = "vya"
[$] "description" @text => "\t为 kiya 全局变量 $mlog 设置锁以保证 `mlog_printf($mlog, ...)` 的线程安全"
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
