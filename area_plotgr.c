/*  area_plotgr.c
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
#include "pgl.h"
#include "primitives.h"
void
bb_add_point (BoundingBox * main, pr_point * add)
{
  if (main->is_first)
    {
      main->ur.x = add->x;
      main->ll.x = add->x;
      main->ur.y = add->y;
      main->ll.y = add->y;
      main->is_first = 0;
    }
  if (main->ur.x < add->x)
    main->ur.x = add->x;
  if (main->ll.x > add->x)
    main->ll.x = add->x;
  if (main->ur.y < add->y)
    main->ur.y = add->y;
  if (main->ll.y > add->y)
    main->ll.y = add->y;
}

void
prp_line_BB (BoundingBox * bBox, PrimBuf prb)
{
  PRIM_LINE_T *data;
  data = (PRIM_LINE_T *) (prb);
  bb_add_point (bBox, &data->a);
  bb_add_point (bBox, &data->b);
}

void
prp_circle_BB (BoundingBox * bBox, PrimBuf prb)
{
  PRIM_CIRCLE_T *data;
  data = (PRIM_CIRCLE_T *) (prb);
}

void
prp_arc_BB (BoundingBox * bBox, PrimBuf prb)
{
  PRIM_ARC_T *data;
  data = (PRIM_ARC_T *) (prb);
}

void
prp_sqr_bezier_BB (BoundingBox * bBox, PrimBuf prb)
{
  PRIM_SQR_BEZIER_T *data;
  data = (PRIM_SQR_BEZIER_T *) (prb);
  bb_add_point (bBox, &data->a);
  bb_add_point (bBox, &data->b);
  bb_add_point (bBox, &data->c);
}

void
prp_step_by_step_BB (BoundingBox * bBox, PglPlot * prb)
{
  int position, len;
  short int mode;
  bBox->is_first = 1;
  len = *(int *) prb;
  position = sizeof (int);
  while (len > position)
    {
      PSI str_size;
      mode = *(PSI *) (prb + position);
      position += sizeof (PSI);
      str_size = *(PSI *) (prb + position);
      position += sizeof (PSI);
      switch (mode)
	{
	case PRIM_LINE:
	  prp_line_BB (bBox, prb + position);
	  break;
	case PRIM_POINT:
	  break;
	case PRIM_CIRCLE:
	  prp_circle_BB (bBox, prb + position);
	  break;
	case PRIM_ARC:
	  prp_arc_BB (bBox, prb + position);
	  break;
	case PRIM_SQR_BEZIER:
	  prp_sqr_bezier_BB (bBox, prb + position);
	  break;
	}
      position += str_size;
    }
}
