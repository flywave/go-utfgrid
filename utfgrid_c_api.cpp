#include "utfgrid_c_api.h"
#include "geometry_impl.hh"

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

void draw_polygon(agg::grid_rasterizer &ras, polygon &pol) {
  for (int i = 0; i < pol.size(); i++) {
    if (i == 0) {
      ras.filling_rule(agg::fill_non_zero);
    } else {
      ras.filling_rule(agg::fill_even_odd);
    }
    linear_ring &r = pol[i];
    for (int p = 0; p < r.size(); p++) {
      if (p == 0) {
        ras.move_to_d(r[p].x, r[p].y);
      } else {
        ras.line_to_d(r[p].x, r[p].y);
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

void _draw_line(utfgrid_t *m, line &l, double width) {
  utfgrid::draw_line(m->ras_grid, l.first.x, l.first.y, l.second.x, l.second.y,
                     width);
}

void _draw_polygon(utfgrid_t *m, polygon &pol) {
  utfgrid::draw_polygon(m->ras_grid, pol);
}

void _draw_ellipse(utfgrid_t *m, point &p, double rx, double ry) {
  utfgrid::draw_ellipse(m->ras_grid, p.x, p.y, rx, ry);
}

void _draw_multi_line(utfgrid_t *m, std::vector<line> &lines, double width) {
  for (int i = 0; i < lines.size(); i++) {
    utfgrid::draw_line(m->ras_grid, lines[i].first.x, lines[i].first.y,
                       lines[i].second.x, lines[i].second.y, width);
  }
}

void _draw_multi_polygon(utfgrid_t *m, std::vector<polygon> &polygons) {
  for (int i = 0; i < polygons.size(); i++) {
    utfgrid::draw_polygon(m->ras_grid, polygons[i]);
  }
}

void _draw_multi_ellipse(utfgrid_t *m, std::vector<point> &points, double rx,
                         double ry) {
  for (int i = 0; i < points.size(); i++) {
    utfgrid::draw_ellipse(m->ras_grid, points[i].x, points[i].y, rx, ry);
  }
}

void _draw_collection(utfgrid_t *m, std::vector<_geometry_t> &geometrys,
                      double *lw, double *pr);

void _draw_geometry(utfgrid_t *m, geometry_t &g, double *lw, double *pr);

wchar_t utfgrid_draw_geometry(utfgrid_t *m, geometry_t *g, double *lw,
                              double *pr) {
  wchar_t key = utfgrid::get_key(m->current++);
  _draw_geometry(m, *g, lw, pr);
  m->ras_grid.render(*m->renderer, key);
  return key;
}

void _draw_geometry(utfgrid_t *m, geometry_t &g, double *lw, double *pr) {
  switch (g.mode) {
  case _POINT:
    _draw_ellipse(m, g.points.front(), *pr, *pr);
    break;
  case _MULTIPOINT:
    _draw_multi_ellipse(m, g.points, *pr, *pr);
    break;
  case _LINESTRING:
    _draw_line(m, g.lines.front(), *lw);
    break;
  case _MULTILINESTRING:
    _draw_multi_line(m, g.lines, *lw);
    break;
  case _POLYGON:
    _draw_polygon(m, g.polygons.front());
    break;
  case _MULTIPOLYGON:
    _draw_multi_polygon(m, g.polygons);
    break;
  case _COLLECTION:
    _draw_collection(m, g.geometrys, lw, pr);
    break;
  default:
    break;
  }
}

void _draw_collection(utfgrid_t *m, std::vector<_geometry_t> &geometrys,
                      double *lw, double *pr) {
  for (int i = 0; i < geometrys.size(); i++) {
    _draw_geometry(m, geometrys[i], lw, pr);
  }
}

const char *utfgrid_to_buf(utfgrid_t *m, int *size) {
  m->outbuf = utfgrid::buf2grid_as_string(4, *m->render_buf);
  *size = m->outbuf.size();
  return reinterpret_cast<const char *>(m->outbuf.c_str());
}

#ifdef __cplusplus
}
#endif
