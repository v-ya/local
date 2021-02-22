: verify   = "*>^~64.4"
: tag      = "kiya"
: version  = "1.0"
: author   = "vya"
[$] "description" @text => @"desc.txt"
: desc     = "description"

[$ "kiya"]          "name"        @string => "import.refer"
[$ "kiya" "parse"]  "kiya.export" @null
[$ "kiya" "dylink" "librefer.so"]
	"refer_alloc"        @null
	"refer_alloz"        @null
	"refer_free"         @null
	"refer_set_free"     @null
	"refer_get_free"     @null
	"refer_save"         @null
	"refer_save_number"  @null

[] "kiya.export" @"kiya.export.all"
