: verify   = "*>^~64.4"
: tag      = "kiya"
: version  = "pupapupa"
: author   = "vya"
[$] "description" @text => @"desc.txt"
: desc     = "description"

[$ "kiya"]          "name"        @string => "import.mlog"
[$ "kiya" "parse"]  "kiya.export" @null
[$ "kiya" "dylink" "libmlog.so"]
	("import-api-by-c-header" "/usr/local/include/mlog.h")

[] "kiya.export" @"kiya.export.all"
