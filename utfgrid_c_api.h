#ifndef GO_UTFGRID_C_API_H
#define GO_UTFGRID_C_API_H

#if defined(WIN32) || defined(WINDOWS) || defined(_WIN32) || defined(_WINDOWS)
#define GOAPICALL __declspec(dllexport)
#else
#define GOAPICALL
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _utfgrid_t utfgrid_t;
typedef struct _geometry_t geometry_t;

GOAPICALL utfgrid_t *utfgrid_new(int width, int height);
GOAPICALL void utfgrid_free(utfgrid_t *m);
GOAPICALL const char *utfgrid_last_error(utfgrid_t *m);
GOAPICALL void utfgrid_reset(utfgrid_t *m);

GOAPICALL wchar_t utfgrid_draw_geometry(utfgrid_t *m, geometry_t *g, double *lw,
                                        double *pr);

GOAPICALL const char *utfgrid_to_buf(utfgrid_t *m, int *size);

#ifdef __cplusplus
}
#endif

#endif
