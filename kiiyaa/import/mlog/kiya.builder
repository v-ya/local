: verify   = "*>^~64.4"
: tag      = "kiya"
: version  = "pupapupa"
: author   = "vya"
[$] "description" @text => @"desc.txt"
: desc     = "description"

[$ "kiya"]          "name"        @string => "import.mlog"
[$ "kiya" "parse"]  "kiya.export" @null
[$ "kiya" "dylink" "libmlog.so"]
	"mlog_alloc"               @null
	"mlog_set_report"          @null
	"mlog_expand"              @null
	"mlog_printf"              @null
	"mlog_clear"               @null
	"mlog_report_stdout_func"  @null

[] "kiya.export" @"kiya.export.all"
