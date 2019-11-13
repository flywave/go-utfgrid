#include "utfgrid_c_api.h"

#include "grid.h"
#include "renderer.h"
#include <math.h>
#include <stdio.h>

#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

namespace utfgrid {

void draw_ellipse(agg::grid_rasterizer &ras, double x, double y, double rx,
                  double ry) {
  ras.filling_rule(agg::fill_non_zero);

  int i;
  ras.move_to_d(x + rx, y);
  for (i = 1; i < 360; i++) {
    double a = double(i) * 3.1415926 / 180.0;
    ras.line_to_d(x + cos(a) * rx, y + sin(a) * ry);
  }
}

void draw_line(agg::grid_rasterizer &ras, double x1, double y1, double x2,
               double y2, double width) {
  ras.filling_rule(agg::fill_non_zero);

  double dx = x2 - x1;
  double dy = y2 - y1;
  double d = sqrt(dx * dx + dy * dy);

  dx = width * (y2 - y1) / d;
  dy = width * (x2 - x1) / d;

  ras.move_to_d(x1 - dx, y1 + dy);
  ras.line_to_d(x2 - dx, y2 + dy);
  ras.line_to_d(x2 + dx, y2 - dy);
  ras.line_to_d(x1 + dx, y1 - dy);
}

void draw_polygon(agg::grid_rasterizer &ras, double **ppoints, int *sizes,
                  int count) {
  for (int i = 0; i < count; i++) {
    int size = sizes[i];
    double *points = ppoints[i];
    if (i == 0) {
      ras.filling_rule(agg::fill_non_zero);
    } else {
      ras.filling_rule(agg::fill_even_odd);
    }
    for (int p = 0; p < size / 2; p++) {
      double *pt = points + (p * 2);
      if (p == 0) {
        ras.move_to_d(pt[0], pt[1]);
      } else {
        ras.line_to_d(pt[0], pt[1]);
      }
    }
  }
}

std::wstring buf2grid_as_string(int step, agg::grid_rendering_buffer &buf) {
  std::wostringstream s(L"");

  s << "{\"grid\":[";
  for (unsigned y = 0; y < buf.height(); y = y + step) {
    agg::grid_value *row = buf.row(y);
    s << "\"";
    for (unsigned x = 0; x < buf.width(); x = x + step) {
      agg::grid_value val = (agg::grid_value)row[x];
      if (val < 0) {
        s << " ";
      } else {
        s << val;
      }
    }

    s << "\"";
    if (y < buf.height() - step) {
      s << ",";
    }
  }
  s << "]}";

  return s.str();
}

char16_t get_key(int index) {
  index++;

  index += 32;

  if (index >= 34)
    index++;

  if (index >= 92)
    index++;

  char16_t st = (char16_t)index;
  return st;
}

} // namespace utfgrid

#ifdef __cplusplus
extern "C" {
#endif

struct _utfgrid_t {
  agg::grid_renderer<agg::span_grid> *renderer;
  agg::grid_value *buf;
  agg::grid_rendering_buffer *render_buf;
  agg::grid_rasterizer ras_grid;
  std::string *err;
  int current;
  std::wstring outbuf;
};

utfgrid_t *utfgrid_new(int width, int height) {
  utfgrid_t *m = new utfgrid_t;
  m->buf = new agg::grid_value[width * height];
  m->render_buf = new agg::grid_rendering_buffer(m->buf, width, height, width);
  m->renderer = new agg::grid_renderer<agg::span_grid>(*m->render_buf);
  m->err = NULL;
  m->current = 0;
  return m;
}

void utfgrid_free(utfgrid_t *m) {
  if (m) {
    if (m->buf) {
      delete m->buf;
    }
    if (m->renderer) {
      delete m->renderer;
    }
    if (m->render_buf) {
      delete m->render_buf;
    }
    if (m->err) {
      delete m->err;
    }
    delete m;
  }
}

const char *utfgrid_last_error(utfgrid_t *m) {
  if (m && m->err) {
    return m->err->c_str();
  }
  return NULL;
}

void utfgrid_reset(utfgrid_t *m) {
  if (m && m->renderer) {
    m->renderer->clear(' ');
    m->current = 0;
  }
}

wchar_t utfgrid_draw_line(utfgrid_t *m, double x1, double y1, double x2,
                          double y2, double width) {
  wchar_t key = utfgrid::get_key(m->current++);
  utfgrid::draw_line(m->ras_grid, x1, y1, x2, y2, width);
  m->ras_grid.render(*m->renderer, key);
  return key;
}

wchar_t utfgrid_draw_polygon(utfgrid_t *m, double **points, int *sizes,
                             int count) {
  wchar_t key = utfgrid::get_key(m->current++);
  utfgrid::draw_polygon(m->ras_grid, points, sizes, count);
  m->ras_grid.render(*m->renderer, key);
  return key;
}

wchar_t utfgrid_draw_ellipse(utfgrid_t *m, double x, double y, double rx,
                             double ry) {
  wchar_t key = utfgrid::get_key(m->current++);
  utfgrid::draw_ellipse(m->ras_grid, x, y, rx, ry);
  m->ras_grid.render(*m->renderer, key);
  return key;
}

wchar_t utfgrid_draw_multi_line(utfgrid_t *m, double *points, int count,
                                double width) {
  wchar_t key = utfgrid::get_key(m->current++);
  for (int i = 0; i < count / 4; i++) {
    double p1x = points[i];
    double p1y = points[i + 1];
    double p2x = points[i + 2];
    double p2y = points[i + 3];
    utfgrid::draw_line(m->ras_grid, p1x, p1y, p2x, p2y, width);
  }
  m->ras_grid.render(*m->renderer, key);
  return key;
}

wchar_t utfgrid_draw_multi_polygon(utfgrid_t *m, double ***points, int **sizes,
                                   int *dims, int count) {
  wchar_t key = utfgrid::get_key(m->current++);
  for (int i = 0; i < count; i++) {
    int *size = sizes[i];
    double **ps = points[i];
    int sdim = dims[i];
    utfgrid::draw_polygon(m->ras_grid, ps, size, sdim);
  }
  m->ras_grid.render(*m->renderer, key);
  return key;
}

wchar_t utfgrid_draw_multi_ellipse(utfgrid_t *m, double *points, int count,
                                   double rx, double ry) {
  wchar_t key = utfgrid::get_key(m->current++);
  for (int i = 0; i < count / 2; i++) {
    double px = points[i * 2];
    double py = points[i * 2 + 1];
    utfgrid::draw_ellipse(m->ras_grid, px, py, rx, ry);
  }
  m->ras_grid.render(*m->renderer, key);
  return key;
}

const char *utfgrid_to_buf(utfgrid_t *m, int *size) {
  m->outbuf = utfgrid::buf2grid_as_string(4, *m->render_buf);
  *size = m->outbuf.size();
  return reinterpret_cast<const char *>(m->outbuf.c_str());
}

#ifdef __cplusplus
}
#endif
