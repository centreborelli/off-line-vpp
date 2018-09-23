/*********************************************************************
 *  Implementation of the Sutherland-Hodgman Polygon Clipping method
 *  Ivan Sutherland, Gary W. Hodgman: Reentrant Polygon Clipping. 
 *            Communications of the ACM, vol. 17, pp. 32–42, 1974
 *  Code from:
 *  https://rosettacode.org/wiki/Sutherland-Hodgman_polygon_clipping#C
 *********************************************************************/

#include "sutherland_hodgman.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


inline float dot(vec a, vec b)
{
	return a->x * b->x + a->y * b->y;
}

inline float cross(vec a, vec b)
{
	return a->x * b->y - a->y * b->x;
}

inline vec vsub(vec a, vec b, vec res)
{
	res->x = a->x - b->x;
	res->y = a->y - b->y;
	return res;
}

/* tells if vec c lies on the left side of directed edge a->b
 * 1 if left, -1 if right, 0 if colinear
 */
int left_of(vec a, vec b, vec c)
{
	vec_t tmp1, tmp2;
	float x;
	vsub(b, a, &tmp1);
	vsub(c, b, &tmp2);
	x = cross(&tmp1, &tmp2);
	return x < 0 ? -1 : x > 0;
}
 
int line_sect(vec x0, vec x1, vec y0, vec y1, vec res)
{
	vec_t dx, dy, d;
	vsub(x1, x0, &dx);
	vsub(y1, y0, &dy);
	vsub(x0, y0, &d);
	float dyx = cross(&dy, &dx);
	if (!dyx) return 0;
	dyx = cross(&d, &dx) / dyx;
	if (dyx <= 0 || dyx >= 1) return 0;
 
	res->x = y0->x + dyx * dy.x;
	res->y = y0->y + dyx * dy.y;
	return 1;
}
 
/* === polygon stuff === */
 
poly poly_new()
{
	return (poly)calloc(1, sizeof(poly_t));
}
 
void poly_free(poly p)
{
	free(p->v);
	free(p);
}
 
void poly_append(poly p, vec v)
{
	if (p->len >= p->alloc) {
		p->alloc *= 2;
		if (!p->alloc) p->alloc = 4;
		p->v = (vec)realloc(p->v, sizeof(vec_t) * p->alloc);
	}
	p->v[p->len++] = *v;
}
 
/* this works only if all of the following are true:
 *   1. poly has no colinear edges;
 *   2. poly has no duplicate vertices;
 *   3. poly has at least three vertices;
 *   4. poly is convex (implying 3).
*/
int poly_winding(poly p)
{
	return left_of(p->v, p->v + 1, p->v + 2);
}
 
void poly_edge_clip(poly sub, vec x0, vec x1, int left, poly res)
{
	int i, side0, side1;
	vec_t tmp;
	vec v0 = sub->v + sub->len - 1, v1;
	res->len = 0;
 
	side0 = left_of(x0, x1, v0);
	if (side0 != -left) poly_append(res, v0);
 
	for (i = 0; i < sub->len; i++) {
		v1 = sub->v + i;
		side1 = left_of(x0, x1, v1);
		if (side0 + side1 == 0 && side0)
			/* last point and current straddle the edge */
			if (line_sect(x0, x1, v0, v1, &tmp))
				poly_append(res, &tmp);
		if (i == sub->len - 1) break;
		if (side1 != -left) poly_append(res, v1);
		v0 = v1;
		side0 = side1;
	}
}
 
poly poly_clip(poly sub, poly clip)
{
	int i;
	poly p1 = poly_new(), p2 = poly_new(), tmp;
 
	int dir = poly_winding(clip);
	poly_edge_clip(sub, clip->v + clip->len - 1, clip->v, dir, p2);
	for (i = 0; i < clip->len - 1; i++) {
		tmp = p2; p2 = p1; p1 = tmp;
		if(p1->len == 0) {
			p2->len = 0;
			break;
		}
		poly_edge_clip(p1, clip->v + i, clip->v + i + 1, dir, p2);
	}
 
	poly_free(p1);
	return p2;
}


int write_polygons(char *name, poly s, poly c, poly res, int nx, int ny)
{
	int i;
 
	/* long and arduous EPS printout */
	FILE * eps = fopen(name, "w");
	fprintf(eps, "%%!PS-Adobe-3.0\n%%%%BoundingBox: 0 0 %d %d\n"
		"/l {lineto} def /m{moveto} def /s{setrgbcolor} def"
		"/c {closepath} def /gs {fill grestore stroke} def\n", nx, ny);
	fprintf(eps, "0 setlinewidth %g %g m ", c->v[0].x, c->v[0].y);

	for (i = 1; i < c->len; i++)
		fprintf(eps, "%g %g l ", c->v[i].x, c->v[i].y);
	fprintf(eps, "c .5 0 0 s gsave 1 .7 .7 s gs\n");
 
	fprintf(eps, "%g %g m ", s->v[0].x, s->v[0].y);
	for (i = 1; i < s->len; i++)
		fprintf(eps, "%g %g l ", s->v[i].x, s->v[i].y);
	fprintf(eps, "c 0 .2 .5 s gsave .4 .7 1 s gs\n");
 
	fprintf(eps, "2 setlinewidth [10 8] 0 setdash %g %g m ",
		res->v[0].x, res->v[0].y);
	for (i = 1; i < res->len; i++)
		fprintf(eps, "%g %g l ", res->v[i].x, res->v[i].y);
	fprintf(eps, "c .5 0 .5 s gsave .7 .3 .8 s gs\n");
 
	fprintf(eps, "%%%%EOF");
	fclose(eps);
 
	return 0;
}



int write_polygons(char *name, poly c, int nx, int ny)
{
	int i;
 
	/* long and arduous EPS printout */
	FILE * eps;
	eps= fopen(name, "w");
	fprintf(eps, "%%!PS-Adobe-3.0\n%%%%BoundingBox: 0 0 %d %d\n"
		"/l {lineto} def /m{moveto} def /s{setrgbcolor} def"
		"/c {closepath} def /gs {fill grestore stroke} def\n", nx, ny);	
	
	fprintf(eps,"newpath\n");

        fprintf(eps, "%g %g m ", c->v[0].x, ny-c->v[0].y);

	for (i = 1; i < c->len; i++)
		fprintf(eps, "%g %g l ", c->v[i].x, ny-c->v[i].y);
		
	fprintf(eps,"closepath \n 2 setlinewidth \n stroke\n");
        fprintf(eps, "%%%%EOF");
	fclose(eps);
 
	return 0;
}
