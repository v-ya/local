miko.major "std.major.void"   = std.fake;
miko.major "std.major.bool"   = miko.real;
miko.major "std.major.int"    = miko.real;
miko.major "std.major.uint"   = miko.real;
miko.major "std.major.float"  = miko.real;
miko.major "std.major.string" = miko.refer;
miko.major "std.major.object" = miko.wink;

std.class "std.major.object";

type.def "void";
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

op.level "access"  =  0;
op.level "unit"    =  1;
op.level "mul"     =  2;
op.level "add"     =  3;
op.level "shift"   =  4;
op.level "compare" =  5;
op.level "bitand"  =  6;
op.level "bitxor"  =  7;
op.level "bitor"   =  8;
op.level "and"     =  9;
op.level "or"      = 10;
op.level "ternary" = 11;
op.level "move"    = 12;

marco.def spec.instr.wrr(_name_, _p1_, _p2_, _p3_) = _name_ {
	instr.access = action.write, _p1_;
	instr.access = action.read,  _p2_;
	instr.access = action.read,  _p3_;
};
marco.def spec.instr.wr(_name_, _p1_, _p2_) = _name_ {
	instr.access = action.write, _p1_;
	instr.access = action.read,  _p2_;
};
marco.def spec.instr.move(_p1_, _p2_) = spec.instr.wr("std.instr.move", _p1_, _p2_);
marco.def spec.instr.jmp(_p1_, _p2_) = "std.instr.jmp" {
	instr.access = action.immed, _p1_;
	instr.access = action.immed, _p2_;
};
marco.def spec.instr.ifnjmp(_p1_, _p2_, _p3_) = "std.instr.ifnjmp" {
	instr.access = action.read,  _p1_;
	instr.access = action.immed, _p2_;
	instr.access = action.immed, _p3_;
};
marco.def spec.instr.op.binary(_name_) = spec.instr.wrr(_name_, op.output, op.left, op.right);
marco.def spec.instr.op.unit(_name_) = spec.instr.wr(_name_, op.output, op.input);

op.binary "." {
	op.priority  = "access";
	op.direction = >>;
};

op.binary "+" {
	op.priority  = "add";
	op.direction = >>;
	op.assembly = {
		op.instr = spec.op.binary.instr("std.instr.add");
	};
};

op.binary "=" {
	op.priority  = "move";
	op.direction = <<;
	op.assembly = {
		op.instr = spec.instr.move(op.left, op.right);
		op.output = op.left;
	};
};

op.unit "~" {
	op.priority  = "unit";
	op.direction = <<;
	op.assembly = {
		op.instr = spec.op.unit.instr("std.instr.bitnot");
	};
};

op.ternary "?" ":" {
	op.priority  = "ternary";
	op.direction = >>;
	op.assembly = {
		op.instr = spec.instr.ifnjmp(op.first, op.label ".label.true", op.label ".label.false");
		op.label = ".label.true";
		op.process = op.inner;
		op.instr = spec.instr.move(op.output, op.inner);
		op.instr = spec.instr.jmp(op.label ".label.false", op.label ".label.end");
		op.label = ".label.false";
		op.process = op.tail;
		op.instr = spec.instr.move(op.output, op.tail);
		op.label = ".label.end";
	};
};
