: verify   = "*>^~64.4"
: tag      = "kiya"
: version  = "motto-motto"
: author   = "vya"
[$] "description" @text => @"desc.txt"
: desc     = "description"

[$ "kiya"]          "name"        @string => "ahaa.slime"
[$ "kiya" "parse"]  "kiya.export" @null
[$ "kiya" "dylink"]
	"ahaa.slime" @"dylink" => &"ahaa.slime.dy"

["kiya.export"]
	"slime_attr"    @null
	"slime_motto"   @null
	"slime_catch"   @null
	"slime_byebye"  @null
	"slime_me"      @null
	"slime_box"     @null
	"slime_box_put" @null
	"slime_box_get" @null

