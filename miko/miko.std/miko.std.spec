miko.major "std.major.void"   = std.fake;
miko.major "std.major.bool"   = miko.real;
miko.major "std.major.int"    = miko.real;
miko.major "std.major.uint"   = miko.real;
miko.major "std.major.float"  = miko.real;
miko.major "std.major.string" = miko.refer;
miko.major "std.major.object" = miko.wink, std.class;

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

marco.def spec.instr.wrr(_name_, _p1_, _p2_, _p3_) = {
	_name_ {
		instr.access = action.write, _p1_;
		instr.access = action.read,  _p2_;
		instr.access = action.read,  _p3_;
	}
};
marco.def spec.instr.wr(_name_, _p1_, _p2_) = {
	_name_ {
		instr.access = action.write, _p1_;
		instr.access = action.read,  _p2_;
	}
};
marco.def spec.instr.move(_p1_, _p2_) = spec.instr.wr("std.instr.move", _p1_, _p2_);
marco.def spec.instr.jdw(_p1_) = {
	"std.instr.jdw" {
		instr.access = action.immed, _p1_;
	}
};
marco.def spec.instr.ifnjdw(_p1_, _p2_) = {
	"std.instr.ifnjdw" {
		instr.access = action.read,  _p1_;
		instr.access = action.immed, _p2_;
	}
};
marco.def spec.instr.op.binary(_name_) = spec.instr.wrr(_name_, op.output, op.left, op.right);
marco.def spec.instr.op.unit(_name_) = spec.instr.wr(_name_, op.output, op.input);

op.binary "." {
	op.priority  = "access";
	op.direction = >>;
	op.output.allow.left;
	op.assembly = {
	};
};

op.unit "[]" {
	op.priority  = "access";
	op.direction = >>;
	op.output.allow.left;
	op.assembly = {
	};
};

op.unit "()" {
	op.priority  = "access";
	op.direction = >>;
	op.assembly = {
	};
};

op.binary "+" {
	op.priority  = "add";
	op.direction = >>;
	op.assembly = {
		op.instr = spec.instr.op.binary("std.instr.add");
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
		op.instr = spec.instr.op.unit("std.instr.bitnot");
	};
};

op.ternary "?" ":" {
	op.priority  = "ternary";
	op.direction = >>;
	op.assembly = {
		op.instr = spec.instr.ifnjdw(op.first, label.diff(".label.false", ".label.true"));
		label.def = ".label.true";
		op.process = op.inner;
		op.instr = spec.instr.move(op.output, op.inner);
		op.instr = spec.instr.jdw(label.diff(".label.end", ".label.false"));
		label.def = ".label.false";
		op.process = op.tail;
		op.instr = spec.instr.move(op.output, op.tail);
		label.def = ".label.end";
	};
};

op.optimization = {
	optimization.instr = "std.instr.move";
	optimization.check = var.is.temp(this.param[1]);
	optimization.done = {
		this.param[1] = this.param[0];
		this.delete;
	};
};
