#ifndef _iyii_desc_h_
#define _iyii_desc_h_

#include "iyii_graph.h"
#include "iyii_source.h"

typedef struct iyii_desc_s {
	graph_descriptor_pool_s *pool;
	graph_descriptor_sets_s *sets;
	graph_descriptor_sets_info_s *info;
} iyii_desc_s;

iyii_desc_s* iyii_desc_alloc(graph_dev_s *restrict dev, iyii_source_s *restrict source, graph_descriptor_set_layout_s *restrict set_layout);

#endif
