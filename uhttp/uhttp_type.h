#ifndef _uhttp_type_h_
#define _uhttp_type_h_

#include "uhttp.h"
#include <rbtree.h>
#include <vattr.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

struct uhttp_inst_s {
	refer_nstring_t version;
	refer_nstring_t empty;
	rbtree_t *status;
};

struct uhttp_s {
	const uhttp_inst_s *inst;
	refer_nstring_t version;
	// request
	refer_nstring_t request_method;
	refer_nstring_t request_uri;
	// response
	intptr_t        status_code;
	refer_nstring_t status_desc;
	// header
	vattr_s *header;
};

struct uhttp_iter_header_id_t {
	vattr_vlist_t iter;
};

struct uhttp_iter_header_all_t {
	vattr_vlist_t iter;
};

#endif
