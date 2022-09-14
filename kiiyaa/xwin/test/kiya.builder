: verify   = "*>^~64.4"
: tag      = "kiya"
: author   = "vya"
[$] "description" @text => "\t测试图形窗口"
: desc     = "description"

[$ "kiya" kiya] "name" @string => "xwin.test"

[kiya "like"]
	"xwin.pool" @null
	"console.main" @null

[kiya "dylink"]
	"xwin.test" @"dylink" => &"xwin.test.dy"

[kiya "parse"]
	"xwin.pool.parse" @null

["xwin.pool.parse" "xwin.test"]
	"title"         @string => "图形窗口测试 (xwin.test)"
	"icon" "size"   @u32    => (u32[2]) { 64, 64 }
	"icon" "data"   @u32    => &"icon_64x64_bgra.pixel"
	"data.allocer"  @string => "data_alloc"
	"events.done"   @string => "events_done"
	"event.close"   @string => "event_close"
	"event.expose"  @string => "event_expose"
	"event.key"     @string => "event_key"
	"event.button"  @string => "event_button"
	"event.pointer" @string => "event_pointer"
	"event.config"  @string => "event_config"
