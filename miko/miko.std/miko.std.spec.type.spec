miko.major "std.major.void"   = std.fake;
miko.major "std.major.bool"   = miko.real;
miko.major "std.major.int"    = miko.real;
miko.major "std.major.uint"   = miko.real;
miko.major "std.major.float"  = miko.real;
miko.major "std.major.string" = miko.refer;
miko.major "std.major.object" = miko.wink;

std.class.major = "std.major.object";

type.def "void";
type.def ".real"   = "std.major.void",   "std.minor.fake.real";
type.def ".refer"  = "std.major.void",   "std.minor.fake.refer";
type.def ".wink"   = "std.major.void",   "std.minor.fake.wink";
type.def "bool"    = "std.major.bool",   "std.minor.bool";
type.def "int8"    = "std.major.int",    "std.minor.bit8";
type.def "uint8"   = "std.major.uint",   "std.minor.bit8";
type.def "int16"   = "std.major.int",    "std.minor.bit16";
type.def "uint16"  = "std.major.uint",   "std.minor.bit16";
type.def "int32"   = "std.major.int",    "std.minor.bit32";
type.def "uint32"  = "std.major.uint",   "std.minor.bit32";
type.def "int64"   = "std.major.int",    "std.minor.bit64";
type.def "uint64"  = "std.major.uint",   "std.minor.bit64";
type.def "int"     = "std.major.int",    "std.minor.bit32";
type.def "uint"    = "std.major.uint",   "std.minor.bit32";
type.def "intptr"  = "std.major.int",    "std.minor.bit32";
type.def "uintptr" = "std.major.uint",   "std.minor.bit32";
type.def "float32" = "std.major.float",  "std.minor.bit32";
type.def "float64" = "std.major.float",  "std.minor.bit64";
type.def "float"   = "std.major.float",  "std.minor.bit32";
type.def "double"  = "std.major.float",  "std.minor.bit64";
type.def "string"  = "std.major.string", "std.minor.string";

