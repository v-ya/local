: verify   = "*>^~64.4"
: tag      = "kiya"
: version  = "kirakira"
: author   = "vya"
[$] "description" @text => @"desc.txt"
: desc     = "description"

[$ "kiya"]          "name"        @string => "import.refer"
[$ "kiya" "parse"]  "kiya.export" @null
[$ "kiya" "dylink" "librefer.so"]
	"refer_alloc"                    @null
	"refer_alloz"                    @null
	"refer_free"                     @null
	"refer_set_free"                 @null
	"refer_get_free"                 @null
	"refer_save"                     @null
	"refer_save_number"              @null
	"refer_dump_string_with_length"  @null
	"refer_dump_string"              @null
	"refer_dump_nstring_with_length" @null
	"refer_dump_nstring"             @null
	"refer_dump_data"                @null

[] "kiya.export" @"kiya.export.all"
