#ifndef _gvcx_model_string_h_
#define _gvcx_model_string_h_

#define gvcx_model_stype__dimensions  ".:dimensions"
#define gvcx_model_sname__dimensions  gvcx_model_type__dimensions

// o gguf
// 	s magic
// 	u version
// 	u n_tensor
// 	u n_metadata_kv
// 	a metadata_kv
// 		o metadata_kv
// 			s metadata_key
// 			e metadata_type
// 			* metadata_value
// 	a tensor_info
// 		o tensor_info
// 			s tensor_name
// 			. tensor_dimensions
// 			e tensor_type
// 			u tensor_offset

#define gvcx_model_stype_gguf__gguf               "o:gguf"
#define gvcx_model_stype_gguf__metadata_kv        "o:gguf.metadata_kv"
#define gvcx_model_stype_gguf__metadata_type      "e:gguf.metadata_type"
#define gvcx_model_stype_gguf__tensor_info        "o:gguf.tensor_info"
#define gvcx_model_stype_gguf__tensor_type        "e:gguf.tensor_type"
#define gvcx_model_sname_gguf__magic              "s:gguf.magic"
#define gvcx_model_sname_gguf__version            "u:gguf.version"
#define gvcx_model_sname_gguf__n_tensor           "u:gguf.n_tensor"
#define gvcx_model_sname_gguf__n_metadata_kv      "u:gguf.n_metadata_kv"
#define gvcx_model_sname_gguf__metadata_kv        "a:gguf.metadata_kv"
#define gvcx_model_sname_gguf__metadata_key       "s:gguf.metadata_key"
#define gvcx_model_sname_gguf__metadata_type      gvcx_model_type_gguf__metadata_type
#define gvcx_model_sname_gguf__metadata_value     "*:gguf.metadata_value"
#define gvcx_model_sname_gguf__tensor_info        "a:gguf.tensor_info"
#define gvcx_model_sname_gguf__tensor_name        "s:gguf.tensor_name"
#define gvcx_model_sname_gguf__tensor_dimensions  gvcx_model__dimensions
#define gvcx_model_sname_gguf__tensor_type        gvcx_model_type_gguf__tensor_type
#define gvcx_model_sname_gguf__tensor_offset      "u:gguf.tensor_offset"

#endif
