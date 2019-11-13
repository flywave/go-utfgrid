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

#ifdef __cplusplus
}
#endif

#endif