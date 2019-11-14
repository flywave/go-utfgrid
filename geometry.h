#ifndef GO_GEOMETRY_C_API_H
#define GO_GEOMETRY_C_API_H

#if defined(WIN32) || defined(WINDOWS) || defined(_WIN32) || defined(_WINDOWS)
#define GEOAPICALL __declspec(dllexport)
#else
#define GEOAPICALL
#endif

#ifdef __cplusplus
extern "C" {
#endif

const int GEO_UNKNOWN = -1;
const int GEO_POINT = 0;
const int GEO_MULTIPOINT = 1;
const int GEO_LINESTRING = 2;
const int GEO_MULTILINESTRING = 3;
const int GEO_POLYGON = 4;
const int GEO_MULTIPOLYGON = 5;
const int GEO_COLLECTION = 6;

typedef struct _geometry_t geometry_t;

GEOAPICALL geometry_t *geometry_new(int type);
GEOAPICALL void geometry_free(geometry_t *m);
GEOAPICALL int geometry_type(geometry_t *m);

GEOAPICALL void add_line(geometry_t *m, double x1, double y1, double x2,
                         double y2);
GEOAPICALL void add_point(geometry_t *m, double x1, double y1);

GEOAPICALL void add_polygon(geometry_t *m, double *points, int count);
GEOAPICALL void add_inner_linear_ring(geometry_t *m, double *points, int count);
GEOAPICALL void add_geometry(geometry_t *m, const geometry_t *geo);

#ifdef __cplusplus
}
#endif

#endif