#include "geometry.h"
#include "geometry_impl.hh"

#include <cassert>

#ifdef __cplusplus
extern "C" {
#endif

geometry_t *geometry_new(int type) {
  geometry_t *m = new geometry_t;
  m->mode = type;
}

void geometry_free(geometry_t *m) {
  if (m) {
    delete m;
  }
}

int geometry_type(geometry_t *m) {
  if (m) {
    return m->mode;
  }
  return GEO_UNKNOWN;
}

void add_line(geometry_t *m, double x1, double y1, double x2, double y2) {
  assert(m->mode == GEO_LINESTRING || m->mode == GEO_MULTILINESTRING);
  if (m->mode == GEO_LINESTRING) {
    if (m->lines.empty()) {
      m->lines.emplace_back(line{point{x1, y1}, point{x2, y2}});
    } else {
      m->lines[0].first.x = x1;
      m->lines[0].first.y = y1;
      m->lines[0].second.x = x2;
      m->lines[0].second.y = y2;
    }
  } else {
    m->lines.emplace_back(line{point{x1, y1}, point{x2, y2}});
  }
}

void add_point(geometry_t *m, double x1, double y1) {
  assert(m->mode == GEO_POINT || m->mode == GEO_MULTIPOINT);
  if (m->mode == GEO_POINT) {
    if (m->points.empty()) {
      m->points.emplace_back(point{x1, y1});
    } else {
      m->points[0].x = x1;
      m->points[0].y = y1;
    }
  } else {
    m->points.emplace_back(point{x1, y1});
  }
}

linear_ring _build_linear_ring(double *points, int count) {
  assert(count % 2 == 0);
  linear_ring ring;
  for (int i = 0; i < count / 2; i++) {
    double px = points[i * 2];
    double py = points[i * 2 + 1];
    ring.emplace_back(point{px, py});
  }
  return std::move(ring);
}

void add_polygon(geometry_t *m, double *points, int count) {
  assert(m->mode == GEO_POLYGON || m->mode == GEO_MULTIPOLYGON);
  if (m->mode == GEO_POLYGON) {
    if (m->polygons.empty()) {
      m->polygons.emplace_back(polygon{_build_linear_ring(points, count)});
    } else {
      if (!m->polygons[0].empty()) {
        m->polygons[0][0] = _build_linear_ring(points, count);
      } else {
        m->polygons[0].emplace_back(_build_linear_ring(points, count));
      }
    }
  } else {
    m->polygons.emplace_back(polygon{_build_linear_ring(points, count)});
  }
}

void add_inner_linear_ring(geometry_t *m, double *points, int count) {
  assert(m->mode == GEO_POLYGON || m->mode == GEO_MULTIPOLYGON);
  m->polygons.end()->emplace_back(_build_linear_ring(points, count));
}

void add_geometry(geometry_t *m, const geometry_t *geo) {
  assert(m->mode == GEO_COLLECTION);
  m->geometrys.emplace_back(geometry_t(*geo));
}

#ifdef __cplusplus
}
#endif
