/*  pgl.h
 *
 * Copyright (C) 2008-2015 Serhii Lysovenko
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __PGL_H__
#define __PGL_H__

typedef double pr_real;
typedef
#ifdef __G_LIB_H__
  struct
{
  pr_real w, h;
  GArray *queue;
}
#else
  void
#endif
  PglPlot;
typedef PglPlot *PrimBuf;
typedef struct
{
  pr_real x, y;
} pr_point;
typedef int pr_color;
typedef int PSI;
typedef struct
{
  double x, y, K;
  int H;
} pr_scale;
typedef struct
{
  pr_scale psc;
  pr_point cur_pt;
} ps_data;
typedef struct
{
  pr_point ll, ur;
  int is_first;
} BoundingBox;
PglPlot *pri_init (pr_real w, pr_real h);
PglPlot *pri_line (PglPlot *, pr_point A, pr_point B);
PglPlot *pri_rectangle (PglPlot *, pr_point A, pr_point B, PSI fill);
PglPlot *pri_circle (PglPlot *, pr_point O, pr_real R);
PglPlot *pri_arc (PglPlot *, pr_point O, pr_real R, pr_real A1, pr_real A2);
PglPlot *pri_sqr_bezier (PglPlot *, pr_point p0, pr_point p1, pr_point p2);
PglPlot *pri_cub_bezier (PglPlot *, pr_point p0, pr_point p1, pr_point p2,
			 pr_point p3);
PglPlot *pri_group_del (PglPlot *, PSI group_id);
PglPlot *pri_group_end (PglPlot *, PSI group_id);
PglPlot *pri_group_start (PglPlot *, PSI group_id);
PglPlot *pri_text (PglPlot * plt, pr_point O, pr_real S, pr_real A1,
		   const char *text, const char *family);
void pri_clear (PglPlot * plt);
#ifdef _STDIO_H
void prp_step_by_step_ps (FILE * psf, pr_scale psc, PglPlot *);
void prp_step_by_step_eps (FILE * psf, pr_scale psc, PglPlot *);
void prp_step_by_step_svg (FILE * svgf, pr_scale psc, PglPlot *);
#endif
void prp_step_by_step_BB (BoundingBox * bBox, PglPlot *);
#ifdef __GDK_H__
void prp_step_by_step_gdk (GdkDrawable * drawable, GdkGC * gc, PglPlot *);
void prp_gdk_pointer (GdkWindow * window, PglPlot *, pr_point *
		      prp, GdkModifierType * state);
#endif
#endif
