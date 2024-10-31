#ifndef _miko_std_marco_h_
#define _miko_std_marco_h_

// iset name

#define miko_std_name  "miko.std"

// iset type

#define miko_std_segment__form       "std.segment.form"       // (exec ) to: instruction
#define miko_std_segment__const      "std.segment.const"      // (init ) by: compiler
#define miko_std_segment__immed      "std.segment.immed"      // (init ) as: instr's immediate number
#define miko_std_action__read        "std.action.read"        // (allow) op: read
#define miko_std_action__write       "std.action.write"       // (allow) op: read write
#define miko_std_action__exec        "std.action.exec"        // (allow) op: call

#define miko_std_major__void         "std.major.void"         // (  fake  ) vtype, index
#define miko_std_major__form         "std.major.form"         // (compiler) stack, const, static, this
#define miko_std_major__int          "std.major.int"          // (  real  ) bit8, bit16, bit32, bit64
#define miko_std_major__uint         "std.major.uint"         // (  real  ) bit8, bit16, bit32, bit64
#define miko_std_major__float        "std.major.float"        // (  real  ) bit32, bit64
#define miko_std_major__string       "std.major.string"       // (  real  ) string
#define miko_std_major__object       "std.major.object"       // (  ipre  ) {dynamic add minor by pre-instruction}

#define miko_std_minor__fake_offset  "std.minor.fake.offset"  // fake: void | std.segment.const
#define miko_std_minor__fake_real    "std.minor.fake.real"    // fake: void | miko.major.vtype.real
#define miko_std_minor__fake_refer   "std.minor.fake.refer"   // fake: void | miko.major.vtype.refer
#define miko_std_minor__fake_wink    "std.minor.fake.wink"    // fake: void | miko.major.vtype.wink
#define miko_std_minor__fake_major   "std.minor.fake.major"   // fake: void | miko.index.major
#define miko_std_minor__fake_minor   "std.minor.fake.minor"   // fake: void | miko.index.minor
#define miko_std_minor__form_stack   "std.minor.form.stack"   // form: form | by: function local var, argv, temp
#define miko_std_minor__form_const   "std.minor.form.const"   // form: form | by: ipre(std.segment.const)
#define miko_std_minor__form_static  "std.minor.form.static"  // form: form | by: static, outside {}, lamda stack
#define miko_std_minor__form_this    "std.minor.form.this"    // form: form | by: object {}
#define miko_std_minor__bit8         "std.minor.bit8"         // real: int uint
#define miko_std_minor__bit16        "std.minor.bit16"        // real: int uint
#define miko_std_minor__bit32        "std.minor.bit32"        // real: int uint float
#define miko_std_minor__bit64        "std.minor.bit64"        // real: int uint float
#define miko_std_minor__string       "std.minor.string"       // real: string

// iset instruction

// ep. refer_nstring_t pre.string(const char *data)
// icpz: instruction.pre.string, 2,
//           { std.segment.form, std.action.write, std.major.string, std.minor.string }, { stack:form_index, form:var_index },
//           { std.segment.const, std.action.read, std.major.void, std.minor.fake.offset }, { runtime:const_index, const:data_offset }

// ep. uint32_t add(uint32_t, uint32_t)
// icpz: instruction.add, 3,
//           { std.segment.form, std.action.write, std.major.uint, std.minor.bit32 }, { stack:form_index, form:var_index },
//           { std.segment.form, std.action.read,  std.major.uint, std.minor.bit32 }, { stack:form_index, form:var_index },
//           { std.segment.form, std.action.read,  std.major.uint, std.minor.bit32 }, { stack:form_index, form:var_index }

// form table
// static by per miko.env
// ep. call function
// [instr] push param
// [instr] fix miko.form.table[stack] and miko.form.table[static]
// [instr] call function
// [instr] fix miko.form.table[stack] and miko.form.table[static]

#endif
