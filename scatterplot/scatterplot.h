#ifndef _scatterplot_h_
#define _scatterplot_h_

#include <refer.h>

typedef struct scatterplot_s scatterplot_s;

scatterplot_s* scatterplot_alloc(void);
void scatterplot_set_image(scatterplot_s *restrict sp, uint32_t w, uint32_t h, uint32_t k, uint32_t color);
void scatterplot_set_view(scatterplot_s *restrict sp, double x1, double y1, double x2, double y2);
void scatterplot_clear(scatterplot_s *restrict sp);
scatterplot_s* scatterplot_pos(scatterplot_s *restrict sp, double x, double y, uint32_t color);
int scatterplot_save(scatterplot_s *restrict sp, const char *restrict path);

#endif
