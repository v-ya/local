: verify   = "*>^~64.4"
: tag      = "kiya"
: version  = "sihasiha"
: author   = "vya"
[$] "description" @text => @"desc.txt"
: desc     = "description"

[$ "kiya"]          "name"        @string => "import.hashmap"
[$ "kiya" "parse"]  "kiya.export" @null
[$ "kiya" "dylink" "libhashmap.so"]
	("import-api-by-c-header" "/usr/local/include/hashmap.h")

[] "kiya.export" @"kiya.export.all"
