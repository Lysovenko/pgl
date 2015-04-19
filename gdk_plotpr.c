/*  gdk_plotpr.c
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

#include <math.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include "pgl.h"
#include "primitives.h"
GdkPoint
rescale (pr_scale psc, pr_point prp)
{
  GdkPoint res;
  res.x = (int) ((prp.x - psc.x) * psc.K + .5);
  res.y = psc.H - (int) ((prp.y - psc.y) * psc.K + .5);
  return res;
}

void
prp_line_gdk (GdkDrawable * drawable, GdkGC * gc, pr_scale psc, void *prb)
{
  GdkColor color;
  GdkPoint a, b;
  PRIM_LINE_T *data;
  data = (PRIM_LINE_T *) (prb);
  color.pixel = data->clr;
  a = rescale (psc, data->a);
  b = rescale (psc, data->b);
  gdk_gc_set_foreground (gc, &color);
  gdk_draw_line (drawable, gc, a.x, a.y, b.x, b.y);
}

void
prp_rectangle_gdk (GdkDrawable * drawable, GdkGC * gc, pr_scale psc,
		   void *prb)
{
  GdkColor color;
  GdkPoint a, b;
  PRIM_RECTANGLE_T *data;
  gint x, y, w, h;
  data = (PRIM_RECTANGLE_T *) (prb);
  color.pixel = data->clr;
  a = rescale (psc, data->a);
  b = rescale (psc, data->b);
  if (a.x < b.x)
    {
      x = a.x;
      w = b.x - a.x;
    }
  else
    {
      x = b.x;
      w = a.x - b.x;
    }
  if (a.y < b.y)
    {
      y = a.y;
      h = b.y - a.y;
    }
  else
    {
      y = b.y;
      h = a.y - b.y;
    }
  gdk_gc_set_foreground (gc, &color);
  gdk_draw_rectangle (drawable, gc, (gboolean) data->fill, x, y, w, h);
}

void
prp_circle_gdk (GdkDrawable * drawable, GdkGC * gc, pr_scale psc, void *prb)
{
  GdkColor color;
  int r;
  pr_point O, A;
  pr_real R;
  GdkPoint a;
  PRIM_CIRCLE_T *data;
  data = (PRIM_CIRCLE_T *) (prb);
  color.pixel = data->clr;
  O = data->o;
  R = data->r;
  A.x = O.x - R;
  A.y = O.y + R;
  a = rescale (psc, A);
  r = (int) (2. * (R * psc.K + .5));
  gdk_gc_set_foreground (gc, &color);
  gdk_draw_arc (drawable, gc, FALSE, a.x, a.y, r, r, 0., 360. * 64.);
}

void
prp_arc_gdk (GdkDrawable * drawable, GdkGC * gc, pr_scale psc, void *prb)
{
  GdkColor color;
  int r;
  pr_point O, A;
  pr_real R, A1, A2;
  GdkPoint a;
  PRIM_ARC_T *data;
  data = (PRIM_ARC_T *) (prb);
  color.pixel = data->clr;
  O = data->o;
  R = data->r;
  A1 = data->alpha;
  A2 = data->beta;
  A.x = O.x - R;
  A.y = O.y + R;
  a = rescale (psc, A);
  r = (int) (2. * (R * psc.K + .5));
  gdk_gc_set_foreground (gc, &color);
  A2 = A2 - A1;
  if (A2 < 0.)
    A2 += 2. * M_PI;
  A1 *= 360. * 64. / M_PI / 2.;
  A2 *= 360. * 64. / M_PI / 2.;
  gdk_draw_arc (drawable, gc, FALSE, a.x, a.y, r, r, A1, A2);
}

void
prp_text_gdk (GdkDrawable * drawable, GdkGC * gc, pr_scale psc, void *prb)
{
  GdkColor color;
  pr_point O;
  pr_real S, A1;
  GdkPoint o;
  PRIM_TEXT_T *data;
  gchar *font;
  cairo_t *cr;
  cairo_pattern_t *pattern;
  data = (PRIM_TEXT_T *) (prb);
  color.pixel = data->clr;
  o = rescale (psc, data->o);
  cr = gdk_cairo_create (drawable);
  cairo_translate (cr, o.x, o.y);
  O.x = 10.;
  O.y = 0.;
  o = rescale (psc, O);
  cairo_scale (cr, o.x / 10, o.x / 10);
  pattern = cairo_pattern_create_linear (-10, -10, 10, 10);
  cairo_pattern_add_color_stop_rgb (pattern, 0., .5, .0, .0);
  cairo_pattern_add_color_stop_rgb (pattern, 1., .0, .0, .5);
  cairo_set_source (cr, pattern);
  O = data->o;
  S = data->s;
  A1 = data->alpha;
  font = g_strdup_printf ("%s 10", data->family);
  g_print ("size=%g; (%g;%g) text=\"%s\" f=\"%s\"\n", S, O.x, O.y, data->text,
	   font);
  g_free (font);
}

void
prp_sqr_bezier_gdk (GdkDrawable * drawable, GdkGC * gc, pr_scale psc,
		    void *prb)
{
  GdkColor color;
  int i, N;
  pr_point p1, p0, p2, O, A;
  GdkPoint a, b;
  PRIM_SQR_BEZIER_T *data;
  data = (PRIM_SQR_BEZIER_T *) (prb);
  color.pixel = data->clr;
  gdk_gc_set_foreground (gc, &color);
  p0 = data->a;
  p1 = data->b;
  p2 = data->c;
  {
    double l1, l2;
    a = rescale (psc, p0);
    b = rescale (psc, p2);
    l1 = (double) a.x - (double) b.x;
    l2 = (double) a.y - (double) b.y;
    N = (int) (sqrt (l1 * l1 + l2 * l2) / 3.);
  }
  N += 1;
  O = p0;
  for (i = 1; i <= N; i++)
    {
      float t, t1, t2, t3;
      t = (float) i / (float) N;
      t1 = 1. - t;
      t2 = t * t;
      t3 = t1 * 2. * t;
      t1 *= t1;
      A.x = t1 * p0.x + t3 * p1.x + t2 * p2.x;
      A.y = t1 * p0.y + t3 * p1.y + t2 * p2.y;
      a = rescale (psc, O);
      b = rescale (psc, A);
      gdk_draw_line (drawable, gc, a.x, a.y, b.x, b.y);
      O = A;
    }
}

void
prp_gdk_pointer (GdkWindow * window, PglPlot * prb, pr_point * prp,
		 GdkModifierType * state)
{
  int H, W, X, Y;
  pr_real h, w, K, x, y;
  gdk_window_get_geometry (window, 0, 0, &W, &H, 0);
  gdk_window_get_pointer (window, &X, &Y, state);
  if (prp && prb)
    {
      h = prb->h;
      w = prb->w;
      if ((pr_real) H / (pr_real) W > h / w)
	{
	  K = (pr_real) W / w;
	  x = 0.;
	  y = -((pr_real) H / K - h) / 2.;
	}
      else
	{
	  K = (pr_real) H / h;
	  x = -((pr_real) W / K - w) / 2.;
	  y = 0.;
	}
      prp->x = (pr_real) X / K + x;
      prp->y = (pr_real) (H - Y) / K + y;
    }
}

static void (*ploters[PRIMITIVES_SIZE])
  (GdkDrawable *, GdkGC *, pr_scale, void *) =
{
NULL, prp_line_gdk, prp_rectangle_gdk, prp_circle_gdk, prp_arc_gdk,
    prp_text_gdk, prp_sqr_bezier_gdk, NULL, NULL};
void
prp_step_by_step_gdk (GdkDrawable * drawable, GdkGC * gc, PglPlot * prb)
{
  int position, len = prb->size;
  pr_scale psc;
  int H, W;
  pr_real h, w;
  h = prb->h;
  w = prb->w;
  gdk_window_get_geometry ((GdkWindow *) drawable, 0, 0, &W, &H, 0);
  if ((pr_real) H / (pr_real) W > h / w)
    {
      psc.K = (pr_real) W / w;
      psc.x = 0.;
      psc.y = -((pr_real) H / psc.K - h) / 2.;
    }
  else
    {
      psc.K = (pr_real) H / h;
      psc.x = -((pr_real) W / psc.K - w) / 2.;
      psc.y = 0.;
    }
  psc.H = H;
  position = 0;
  while (len > position)
    {
      PSI str_size = ((PRIM_ITEM_T *) (prb->data + position))->size,
	type = ((PRIM_ITEM_T *) (prb->data + position))->type;
      void (*ploter) (GdkDrawable *, GdkGC *, pr_scale, void *) = NULL;
      /* ensure array index to be correct */
      if (type >= 0 && type < PRIMITIVES_SIZE)
	ploter = ploters[type];
      position += sizeof (PRIM_ITEM_T);
      if (ploter)
	ploter (drawable, gc, psc, prb->data + position);
      position += str_size;
    }
}
