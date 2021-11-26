: verify   = "*>^~64.4"
: tag      = "kiya"
: author   = "vya"
[$] "description" @text => "\t"
: desc     = "description"

[$ "kiya"] "name" @string => "console.cmd.web-server"

[$ "kiya" "like"]
	"console.main" @null
	"web.server" @null

[$ "kiya" "dylink"]
	"console.cmd.web-server" @"dylink" => &"console.cmd.web-server.dy"

[$ "kiya" "parse"]
	"console.main.parse.command" @null

["console.main.parse.command"]
	"ws.limit" @string => "command_alloc_limit"
	"ws.status" @string => "command_alloc_status"
