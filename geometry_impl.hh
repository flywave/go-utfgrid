#ifndef GO_GEOMETRY_IMPL_HH
#define GO_GEOMETRY_IMPL_HH

#include <vector>

const int _UNKNOWN = -1;
const int _POINT = 0;
const int _MULTIPOINT = 1;
const int _LINESTRING = 2;
const int _MULTILINESTRING = 3;
const int _POLYGON = 4;
const int _MULTIPOLYGON = 5;
const int _COLLECTION = 6;

struct point {
  double x;
  double y;
};

typedef std::pair<point, point> line;
typedef std::vector<point> linear_ring;
typedef std::vector<linear_ring> polygon;

struct _geometry_t {
  int mode;
  std::vector<point> points;
  std::vector<line> lines;
  std::vector<polygon> polygons;
  std::vector<_geometry_t> geometrys;
};

#endif