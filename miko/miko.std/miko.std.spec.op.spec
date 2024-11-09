op.priority "access"  =  0;
op.priority "unit"    =  1;
op.priority "mul"     =  2;
op.priority "add"     =  3;
op.priority "shift"   =  4;
op.priority "compare" =  5;
op.priority "bitand"  =  6;
op.priority "bitxor"  =  7;
op.priority "bitor"   =  8;
op.priority "and"     =  9;
op.priority "or"      = 10;
op.priority "ternary" = 11;
op.priority "move"    = 12;

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

