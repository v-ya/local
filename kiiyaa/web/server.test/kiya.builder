: verify   = "*>^~64.4"
: tag      = "kiya"
: author   = "vya"
[$] "description" @text => "\t简单的网站测试 127.0.0.1:8080"
: desc     = "description"

[$ "kiya" kiya] "name" @string => "web.server.test"

[kiya "like"]
	"web.server" @null
	"web.server.preset-tag" @null
	"console.main" @null
	"console.cmd.web-server" @null
	"import.rbtree" @null

[kiya "dylink" dylink]
	[dylink "libc.so.6"]
		"strcmp" @null
	[dylink] "web.server.test" @"dylink" => &"web.server.test.dy"

[kiya "parse"]
	"web.server.parse" @null
	"console.main.parse.command" @null

[kiya]
	"finally" @string => "finally"

["web.server.parse" parse]
[parse "header"]
	# 注册 header
	"Server" @text => "web.server.test by kiya (vya)"
	"Connection" @string => "header_alloc_connection"
[parse "trigger"]
	"web.server.test.statistics" @string => "trigger_alloc_statistics"
[parse "register" register]
[register "#"]
	# 注册响应返回路由
	"404" @"body.data|mini" => &"domain/404.html"
[register "#method"]
	# 设置注册方法
	"+GET" @null
	"+POST" @null
[register]
	# 注册请求路由
	"favicon.ico" @"body.data|mini" => &"domain/favicon.ico"
	"index.html" @"body.data|mini" => &"domain/index.html"
	# 之后链接到 /index.html
	"" @text => "GET:index.html"
	"index" @text => "GET:index.html"
	"404" @text => "#404"

# /favicon.ico => domain/favicon.ico
# /index.html  => domain/index.html
# /            => domain/index.html
# /index       => domain/index.html
# /404         => domain/404.html
# #404         => domain/404.html

[parse "bind"]
	"localhost:8080" @string => "127.0.0.1:8080"

["console.main.parse.command"]
	"stat" @string => "command_alloc_statistics"
