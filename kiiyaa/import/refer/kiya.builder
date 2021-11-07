: verify   = "*>^~64.4"
: tag      = "kiya"
: version  = "kirakira"
: author   = "vya"
[$] "description" @text => @"desc.txt"
: desc     = "description"

[$ "kiya"]          "name"        @string => "import.refer"
[$ "kiya" "parse"]  "kiya.export" @null
[$ "kiya" "dylink" "librefer.so"]
	("import-api-by-c-header" "/usr/local/include/refer.h")

[] "kiya.export" @"kiya.export.all"
