#ifndef _miko_std_marco_h_
#define _miko_std_marco_h_

// iset name

#define miko_std_name  "miko.std"

// iset type

#define miko_std_segment__stack      "std.segment.stack"      // by: function local var, argv, temp
#define miko_std_segment__static     "std.segment.static"     // by: static, const, outside {}, lamda ref stack
#define miko_std_segment__this       "std.segment.this"       // by: object {}
#define miko_std_action__immed       "std.action.immed"       // (allow) op: instr
#define miko_std_action__read        "std.action.read"        // (allow) op: read
#define miko_std_action__write       "std.action.write"       // (allow) op: read write
#define miko_std_action__exec        "std.action.exec"        // (allow) op: call

#define miko_std_major__void         "std.major.void"         // (fake) vtype, index
#define miko_std_major__form         "std.major.form"         // (wink) form
#define miko_std_major__int          "std.major.int"          // (real) bit8, bit16, bit32, bit64
#define miko_std_major__uint         "std.major.uint"         // (real) bit8, bit16, bit32, bit64
#define miko_std_major__float        "std.major.float"        // (real) bit32, bit64
#define miko_std_major__string       "std.major.string"       // (real) string
#define miko_std_major__object       "std.major.object"       // (ipre) {dynamic add minor by pre-instruction}

#define miko_std_minor__fake_offset  "std.minor.fake.offset"  // fake: void | std.segment.const
#define miko_std_minor__fake_real    "std.minor.fake.real"    // fake: void | miko.major.vtype.real
#define miko_std_minor__fake_refer   "std.minor.fake.refer"   // fake: void | miko.major.vtype.refer
#define miko_std_minor__fake_wink    "std.minor.fake.wink"    // fake: void | miko.major.vtype.wink
#define miko_std_minor__fake_major   "std.minor.fake.major"   // fake: void | miko.index.major
#define miko_std_minor__fake_minor   "std.minor.fake.minor"   // fake: void | miko.index.minor
#define miko_std_minor__form         "std.minor.form"         // form: wink | at miko.form.table[static]
#define miko_std_minor__bit8         "std.minor.bit8"         // real: int uint
#define miko_std_minor__bit16        "std.minor.bit16"        // real: int uint
#define miko_std_minor__bit32        "std.minor.bit32"        // real: int uint float
#define miko_std_minor__bit64        "std.minor.bit64"        // real: int uint float
#define miko_std_minor__string       "std.minor.string"       // real: string

// iset instruction

// form table
// static by per miko.env
// ep. call function
// [instr] push param
// [instr] push and switch env:
//             push curr miko.form.table[static]
//             push curr miko.form.table[this]
//             push curr instr-list and instr-pos
//             set  next miko.form.table[static]
//             set  next miko.form.table[this]
//             set  next instr-list and instr-pos
// [instr] pop env:
//             pop  last miko.form.table[static]
//             pop  last miko.form.table[this]
//             pop  last instr-list and instr-pos
// [instr] call function
// [instr] fix miko.form.table[stack] and miko.form.table[static]

#endif
