#ifndef _layer_model_string_h_
#define _layer_model_string_h_

// *: any
//-u: uint64_t
//-i: int64_t
//-f: double
//-b: uint64_t (0:false|1:true)
//-s: string (refer_nstring_t:utf-8)
//-d: data (refer_nstring_t|layer_file_s:ref)
//+e: enum (rbtree: uint64_t => string)
//+a: array
//+o: object
//+.: preset

#define layer_model_stype__null        "!"
#define layer_model_stype__any         "*"
#define layer_model_stype__uint        "u"
#define layer_model_stype__int         "i"
#define layer_model_stype__float       "f"
#define layer_model_stype__boolean     "b"
#define layer_model_stype__string      "s"
#define layer_model_stype__data        "d"
#define layer_model_stype__enum        "e"
#define layer_model_stype__array       "a"
#define layer_model_stype__object      "o"
#define layer_model_stype__preset      "."

#endif
