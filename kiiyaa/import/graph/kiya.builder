: verify   = "*>^~64.4"
: tag      = "kiya"
: version  = "fuwafuwa"
: author   = "vya"
[$] "description" @text => @"desc.txt"
: desc     = "description"

[$ "kiya"]          "name"        @string => "import.graph"
[$ "kiya" "parse"]  "kiya.export" @null
[$ "kiya" "dylink" "libgraph.so"]
	("import-api-by-c-header" "/usr/local/include/graph/graph.h")
	("import-api-by-c-header" "/usr/local/include/graph/allocator.h")
	("import-api-by-c-header" "/usr/local/include/graph/support.h")
	("import-api-by-c-header" "/usr/local/include/graph/device.h")
	("import-api-by-c-header" "/usr/local/include/graph/surface.h")
	("import-api-by-c-header" "/usr/local/include/graph/image.h")
	("import-api-by-c-header" "/usr/local/include/graph/layout.h")
	("import-api-by-c-header" "/usr/local/include/graph/shader.h")
	("import-api-by-c-header" "/usr/local/include/graph/command.h")
	("import-api-by-c-header" "/usr/local/include/graph/buffer.h")

[] "kiya.export" @"kiya.export.all"
