: verify   = "*>^~64.4"
: tag      = "kiya"
: version  = "(^_^)"
: author   = "vya"

[$ "kiya"]
	"name" @string => "import-api-by-c-header"
	"initial" @string => "initial"
	"finally" @string => "finally"

[$ "kiya" "dylink" "librefer.so"]
	"refer_free" @null
[$ "kiya" "dylink" "libmlog.so"]
	"mlog_printf" @null
[$ "kiya" "dylink" "libhashmap.so"]
	"hashmap_init" @null
	"hashmap_uini" @null
	"hashmap_find_name" @null
	"hashmap_set_name" @null
[$ "kiya" "dylink" "libvattr.so"]
	"vattr_delete" @null
	"vattr_get_vslot" @null
	"vattr_next" @null
	"vattr_next_end" @null
	"vattr_last" @null
	"vattr_last_end" @null
	"vattr_vslot_first" @null
	"vattr_vslot_next" @null
	"vattr_vslot_next_end" @null
[$ "kiya" "dylink" "libpocket-saver.so"]
	"pocket_saver_create_null" @null
[$ "kiya" "dylink" "libcparse.so"]
	"cparse_inst_alloc" @null
	"cparse_scope_load" @null

[$ "kiya" "dylink"]
	"import-api-by-c-header" @"dylink" => &"../import-api-by-c-header.dy"

[$ "kiya" "parse"]
	"kiya.export" @null

["kiya.export"]
	"kmain"   @string => "import-api-by-c-header"
