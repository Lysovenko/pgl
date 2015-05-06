/*  svg_plotpr.c
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
#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "pgl.h"
#include "primitives.h"
pr_point
prp_2svg (pr_scale psc, pr_point prp)
{
  pr_point res;
  res.x = (psc.x + prp.x) * psc.K;
  res.y = (double) psc.H - (psc.y + prp.y) * psc.K;
  return res;
}

void
prp_line_svg (FILE * svgf, pr_scale psc, void *prb)
{
  pr_point A, B;
  PRIM_LINE_T *data;
  data = (PRIM_LINE_T *) (prb);
  A = prp_2svg (psc, data->a);
  B = prp_2svg (psc, data->b);
  {
    fprintf (svgf, "<line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\"/>\n",
	     A.x, A.y, B.x, B.y);
  }
}

void
prp_circle_svg (FILE * svgf, pr_scale psc, void *prb)
{
  pr_point O, A;
  pr_real R;
  PRIM_CIRCLE_T *data = (PRIM_CIRCLE_T *) (prb);
  A.x = O.x - R;
  A.y = O.y + R;
}

void
prp_arc_svg (FILE * svgf, pr_scale psc, void *prb)
{
  int r;
  pr_point O, A;
  pr_real R, A1, A2;
  PRIM_ARC_T *data = (PRIM_ARC_T *) (prb);
  A.x = O.x - R;
  A.y = O.y + R;
  r = (int) (2. * (R * psc.K + .5));
  A2 = A2 - A1;
  if (A2 < 0.)
    A2 += 2. * M_PI;
  A1 *= 360. * 64. / M_PI / 2.;
  A2 *= 360. * 64. / M_PI / 2.;
}

void
prp_sqr_bezier_svg (FILE * svgf, pr_scale psc, void *prb)
{
  pr_point p1, p0, p2 /* , O, A, cp1, cp2 */ ;
  PRIM_SQR_BEZIER_T *data;
  data = (PRIM_SQR_BEZIER_T *) (prb);
  p0 = prp_2svg (psc, data->a);
  p1 = prp_2svg (psc, data->b);
  p2 = prp_2svg (psc, data->c);
  {
    fprintf (svgf,
	     "<path d=\"M %lg %lg Q %lg %lg  %lg %lg\" fill=\"none\"/>\n",
	     p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);
  }
}

static void (*ploters[PRIMITIVES_SIZE]) (FILE *, pr_scale, void *) =
{
NULL, prp_line_svg, NULL, prp_circle_svg, prp_arc_svg,
    NULL, prp_sqr_bezier_svg, NULL, NULL, NULL};

void
prp_step_by_step_svg (FILE * svgf, pr_scale psc, PglPlot * prb)
{
  int position;
  GArray *queue = prb->queue;
  setlocale (LC_NUMERIC, "C");
  for (position = 0; position < queue->len; position++)
    {
      PRIM_ITEM_T item = g_array_index (queue, PRIM_ITEM_T, position);
      void (*ploter) (FILE *, pr_scale, void *) = NULL;
      if (item.type >= 0 && item.type < PRIMITIVES_SIZE)
	ploter = ploters[item.type];
      if (ploter)
	ploter (svgf, psc, item.data);
    }
  setlocale (LC_NUMERIC, "");
}
