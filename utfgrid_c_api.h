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

GOAPICALL utfgrid_t *utfgrid_new(int width, int height);
GOAPICALL void utfgrid_free(utfgrid_t *m);
GOAPICALL const char *utfgrid_last_error(utfgrid_t *m);
GOAPICALL void utfgrid_reset(utfgrid_t *m);

GOAPICALL wchar_t utfgrid_draw_line(utfgrid_t *m, double x1, double y1,
                                    double x2, double y2, double width);
GOAPICALL wchar_t utfgrid_draw_multi_line(utfgrid_t *m, double *points,
                                          int count, double width);
GOAPICALL wchar_t utfgrid_draw_polygon(utfgrid_t *m, double **points,
                                       int *sizes, int count);
GOAPICALL wchar_t utfgrid_draw_ellipse(utfgrid_t *m, double x, double y,
                                       double rx, double ry);
GOAPICALL wchar_t utfgrid_draw_multi_polygon(utfgrid_t *m, double ***points,
                                             int **sizes, int *dims, int count);
GOAPICALL wchar_t utfgrid_draw_multi_ellipse(utfgrid_t *m, double *points,
                                             int count, double rx, double ry);

GOAPICALL const char *utfgrid_to_buf(utfgrid_t *m, int *size);

#ifdef __cplusplus
}
#endif

#endif
