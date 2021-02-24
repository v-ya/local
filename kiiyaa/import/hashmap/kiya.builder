: verify   = "*>^~64.4"
: tag      = "kiya"
: version  = "sihasiha"
: author   = "vya"
[$] "description" @text => @"desc.txt"
: desc     = "description"

[$ "kiya"]          "name"        @string => "import.hashmap"
[$ "kiya" "parse"]  "kiya.export" @null
[$ "kiya" "dylink" "libhashmap.so"]
	"hashmap_init"         @null
	"hashmap_uini"         @null
	"hashmap_clear"        @null
	"hashmap_alloc"        @null
	"hashmap_free"         @null
	"hashmap_find_name"    @null
	"hashmap_find_head"    @null
	"hashmap_delete_name"  @null
	"hashmap_delete_head"  @null
	"hashmap_put_name"     @null
	"hashmap_put_head"     @null
	"hashmap_set_name"     @null
	"hashmap_set_head"     @null
	"hashmap_get_name"     @null
	"hashmap_get_head"     @null
	"hashmap_call"         @null
	"hashmap_call_v2"      @null
	"hashmap_isfree"       @null

[] "kiya.export" @"kiya.export.all"
