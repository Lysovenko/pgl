/*  ps_plotpr.c
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
#include <glib.h>
#include <stdio.h>
#include <glib/gprintf.h>
#include <locale.h>
#include "pgl.h"
#include "primitives.h"
static pr_point
prp_2ps (pr_scale psc, pr_point prp)
{
  pr_point res;
  res.x = (psc.x + prp.x) * psc.K;
  res.y = (psc.y + prp.y) * psc.K;
  return res;
}

static void
prp_line_ps (FILE * psf, ps_data * psd, void *prb)
{
  PRIM_LINE_T *data = (PRIM_LINE_T *) (prb);
  pr_point A, B;
  A = prp_2ps (psd->psc, data->a);
  B = prp_2ps (psd->psc, data->b);
  {
    g_fprintf (psf, "%g %g moveto\n", A.x, A.y);
    g_fprintf (psf, "%g %g lineto\n", B.x, B.y);
    psd->cur_pt = B;
  }
}

static void
prp_circle_ps (FILE * psf, ps_data * psd, void *prb)
{
  /* A.x = O.x - R; */
  /* A.y = O.y + R; */
}

static void
prp_arc_ps (FILE * psf, ps_data * psd, void *prb)
{
/* TODO: Make it working */
  pr_point O = { 0, 0 }, A;
  PRIM_ARC_T *data = (PRIM_ARC_T *) (prb);
  pr_real R = data->r, A1 = data->alpha, A2 = data->beta;
  A.x = O.x - R;
  A.y = O.y + R;
  A2 = A2 - A1;
  if (A2 < 0.)
    A2 += 2. * M_PI;
  A1 *= 360. * 64. / M_PI / 2.;
  A2 *= 360. * 64. / M_PI / 2.;
  g_fprintf (psf, "%% arc %g %g %g %g Not Implemented\n", A.x, A.y, A1, A2);
}

static void
prp_sqr_bezier_ps (FILE * psf, ps_data * psd, void *prb)
{
  pr_point p1, p0, p2, cp1, cp2;
  PRIM_SQR_BEZIER_T *data;
  data = (PRIM_SQR_BEZIER_T *) (prb);
  p0 = prp_2ps (psd->psc, data->a);
  p1 = prp_2ps (psd->psc, data->b);
  p2 = prp_2ps (psd->psc, data->c);
  cp1.x = p0.x + 2. / 3. * (p1.x - p0.x);
  cp1.y = p0.y + 2. / 3. * (p1.y - p0.y);
  cp2.x = cp1.x + 1. / 3. * (p2.x - p0.x);
  cp2.y = cp1.y + 1. / 3. * (p2.y - p0.y);
  {
    g_fprintf (psf, "%g %g moveto\n", p0.x, p0.y);
    g_fprintf (psf, "%g %g  %g %g  %g %g curveto\n", cp1.x, cp1.y, cp2.x,
	       cp2.y, p2.x, p2.y);
    psd->cur_pt = p2;
  }
}

static void
prp_cub_bezier_ps (FILE * psf, ps_data * psd, void *prb)
{
  pr_point p1, p0, p2, p3;
  PRIM_CUB_BEZIER_T *data;
  data = (PRIM_CUB_BEZIER_T *) (prb);
  p0 = prp_2ps (psd->psc, data->a);
  p1 = prp_2ps (psd->psc, data->b);
  p2 = prp_2ps (psd->psc, data->c);
  p3 = prp_2ps (psd->psc, data->d);
  {
    g_fprintf (psf, "%g %g moveto\n", p0.x, p0.y);
    g_fprintf (psf, "%g %g  %g %g  %g %g curveto\n", p1.x, p1.y, p2.x, p2.y,
	       p3.x, p3.y);
    psd->cur_pt = p3;
  }
}

static void (*ploters[PRIMITIVES_SIZE]) (FILE *, ps_data *, void *) =
{
NULL, prp_line_ps, NULL, prp_circle_ps, prp_arc_ps,
    NULL, prp_sqr_bezier_ps, prp_cub_bezier_ps, NULL, NULL};
void
prp_step_by_step_ps (FILE * psf, pr_scale psc, PglPlot * prb)
{
  int position = 0;
  GArray *queue = prb->queue;
  ps_data psd;
  psd.psc = psc;
  psd.cur_pt.x = 1e100;
  psd.cur_pt.y = 1e100;
  setlocale (LC_NUMERIC, "C");
  fputs ("newpath\n", psf);
  while (queue->len > position)
    {
      PRIM_ITEM_T item = g_array_index (queue, PRIM_ITEM_T, position);
      void (*ploter) (FILE *, ps_data *, void *) = NULL;
      if (item.type >= 0 && item.type < PRIMITIVES_SIZE)
	ploter = ploters[item.type];
      if (ploter)
	ploter (psf, &psd, item.data);
      position++;
    }
  fputs ("1 setlinewidth\nstroke\nshowpage\n", psf);
  setlocale (LC_NUMERIC, "");
}

void
prp_step_by_step_eps (FILE * psf, pr_scale psc, PglPlot * prb)
{
  int position = 0;
  GArray *queue = prb->queue;
  BoundingBox bBox;
  ps_data psd;
  psd.psc = psc;
  psd.cur_pt.x = 1e100;
  psd.cur_pt.y = 1e100;
  setlocale (LC_NUMERIC, "C");
  prp_step_by_step_BB (&bBox, prb);
  psd.psc.x = -bBox.ll.x;
  psd.psc.y = -bBox.ll.y;
  g_fprintf (psf,
	     "%%!PS-Adobe-2.0 EPSF-2.0\n%%%%Creator: pgl\n"
	     "%%%%BoundingBox: 0 0 %g %g\n%%%%EndComments\n",
	     (bBox.ur.x + psd.psc.x) * psd.psc.K,
	     (bBox.ur.y + psd.psc.y) * psd.psc.K);
  fputs ("newpath\n", psf);
  while (queue->len > position)
    {
      PRIM_ITEM_T item = g_array_index (queue, PRIM_ITEM_T, position);
      void (*ploter) (FILE *, ps_data *, void *) = NULL;
      /* ensure array index to be correct */
      if (item.type >= 0 && item.type < PRIMITIVES_SIZE)
	ploter = ploters[item.type];
      if (ploter)
	ploter (psf, &psd, item.data);
      position++;
    }
  fputs ("1 setlinewidth\nstroke\n", psf);
  setlocale (LC_NUMERIC, "");
}
