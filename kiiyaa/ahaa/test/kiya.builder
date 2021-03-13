: tag      = "kiya"

[$ "kiya"]          "name"        @string => "ahaa.test"

[$ "kiya" "like"]
	# test need kiya pocket

[$ "kiya" "parse"]  "kiya.export" @null

[$ "kiya" "dylink"]
	"ahaa.test" @"dylink" => &"ahaa.test.dy"

["kiya.export"]
	"main_do"   @string => "main"
