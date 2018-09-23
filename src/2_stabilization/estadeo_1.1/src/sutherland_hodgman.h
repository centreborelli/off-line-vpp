#ifndef SUTHERLAND_HODGMAN
#define SUTHERLAND_HODGMAN

typedef struct { float x, y; } vec_t, *vec;
typedef struct { int len, alloc; vec v; } poly_t, *poly;

poly poly_new();
void poly_free(poly p);

poly poly_clip(poly sub, poly clip);
int write_polygons(char *name, poly s, poly c, poly res, int nx, int ny);
int write_polygons(char *name, poly c, int nx, int ny);

#endif