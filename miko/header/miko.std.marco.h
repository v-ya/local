#ifndef _miko_std_marco_h_
#define _miko_std_marco_h_

// iset name

#define miko_std_name  "miko.std"

// iset type

#define miko_std_score__const        "std.score.const"        // (init ) by: compiler
#define miko_std_score__form         "std.score.form"         // (exec ) to: instruction
#define miko_std_style__read         "std.style.read"         // (allow) op: read
#define miko_std_style__write        "std.style.write"        // (allow) op: read write
#define miko_std_style__exec         "std.style.exec"         // (allow) op: call

#define miko_std_major__void         "std.major.void"         // (  fake  ) vtype, index
#define miko_std_major__form         "std.major.form"         // (compiler) stack, const, static, this
#define miko_std_major__int          "std.major.int"          // (  real  ) bit8, bit16, bit32, bit64
#define miko_std_major__uint         "std.major.uint"         // (  real  ) bit8, bit16, bit32, bit64
#define miko_std_major__float        "std.major.float"        // (  real  ) bit32, bit64
#define miko_std_major__string       "std.major.string"       // (  real  ) string
#define miko_std_major__object       "std.major.object"       // (  ipre  ) {dynamic add minor by pre-instruction}

#define miko_std_minor__fake_offset  "std.minor.fake.offset"  // fake: void | std.score.const
#define miko_std_minor__fake_real    "std.minor.fake.real"    // fake: void | miko.major.vtype.real
#define miko_std_minor__fake_refer   "std.minor.fake.refer"   // fake: void | miko.major.vtype.refer
#define miko_std_minor__fake_wink    "std.minor.fake.wink"    // fake: void | miko.major.vtype.wink
#define miko_std_minor__fake_major   "std.minor.fake.major"   // fake: void | miko.index.major
#define miko_std_minor__fake_minor   "std.minor.fake.minor"   // fake: void | miko.index.minor
#define miko_std_minor__form_stack   "std.minor.form.stack"   // form: form | by: function local var, argv, temp
#define miko_std_minor__form_const   "std.minor.form.const"   // form: form | by: ipre(std.score.const)
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
//           { std.score.form, std.style.write, std.major.string, std.minor.string }, { stack:form_index, form:var_index },
//           { std.score.const, std.style.read, std.major.void, std.minor.fake.offset }, { runtime:const_index, const:data_offset }

// ep. uint32_t add(uint32_t, uint32_t)
// icpz: instruction.add, 3,
//           { std.score.form, std.style.write, std.major.uint, std.minor.bit32 }, { stack:form_index, form:var_index },
//           { std.score.form, std.style.read,  std.major.uint, std.minor.bit32 }, { stack:form_index, form:var_index },
//           { std.score.form, std.style.read,  std.major.uint, std.minor.bit32 }, { stack:form_index, form:var_index }

#endif
