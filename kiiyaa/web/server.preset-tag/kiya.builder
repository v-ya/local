: verify   = "*>^~64.4"
: tag      = "kiya"
: author   = "vya"
[$] "description" @text => "\t提供 web.server 的一些预设 tag"
: desc     = "description"

[$ "kiya"] "name" @string => "web.server.preset-tag"

[$ "kiya" "like"]
	"web.server" @null
	"import.fsys" @null

[$ "kiya" "dylink"]
	"web.server.preset-tag" @"dylink" => &"web.server.preset-tag.dy"

[$ "kiya" "parse"]
	"web.server.parse" @null

["web.server.parse" "tag.global"]
	"body.data"            @string => "body_data_builder"
	"body.data.mini"       @string => "body_data_builder"
	"body.data.close"      @string => "body_data_builder"
	"body.data.mini.close" @string => "body_data_builder"
	"body.path"            @string => "body_path_builder"
	"body.path.close"      @string => "body_path_builder"
