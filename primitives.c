/*  primitives.c
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
#include <string.h>
#include "primitives.h"
PglPlot *
pri_init (pr_real w, pr_real h)
{
  PglPlot *plt;
  plt = g_malloc (sizeof (PglPlot));
  plt->size = 0;
  plt->data = NULL;
  plt->w = w;
  plt->h = h;
  return plt;
}

PglPlot *
pri_line (PglPlot * plt, pr_point A, pr_point B)
{
  int size;
  PRIM_LINE_T *data;
  size = plt->size;
  plt->data =
    g_realloc (plt->data, size + sizeof (PRIM_ITEM_T) + sizeof (PRIM_LINE_T));
  ((PRIM_ITEM_T *) (plt->data + size))->type = PRIM_LINE;
  ((PRIM_ITEM_T *) (plt->data + size))->size = sizeof (PRIM_LINE_T);
  size += sizeof (PRIM_ITEM_T);
  data = (plt->data + size);
  data->a = A;
  data->b = B;
  size += sizeof (PRIM_LINE_T);
  plt->size = size;
  return plt;
}

PglPlot *
pri_rectangle (PglPlot * plt, pr_point A, pr_point B, PSI fill)
{
  int size;
  PRIM_RECTANGLE_T *data;
  size = plt->size;
  plt->data =
    g_realloc (plt->data,
	       size + sizeof (PRIM_ITEM_T) + sizeof (PRIM_RECTANGLE_T));
  ((PRIM_ITEM_T *) (plt->data + size))->type = PRIM_RECTANGLE;
  ((PRIM_ITEM_T *) (plt->data + size))->size = sizeof (PRIM_RECTANGLE_T);
  size += sizeof (PRIM_ITEM_T);
  data = (plt->data + size);
  data->a = A;
  data->b = B;
  data->fill = fill;
  size += sizeof (PRIM_RECTANGLE_T);
  plt->size = size;
  return plt;
}

PglPlot *
pri_circle (PglPlot * plt, pr_point O, pr_real R)
{
  int size;
  PRIM_CIRCLE_T *data;
  size = plt->size;
  plt->data =
    g_realloc (plt->data,
	       size + sizeof (PRIM_ITEM_T) + sizeof (PRIM_CIRCLE_T));
  ((PRIM_ITEM_T *) (plt->data + size))->type = PRIM_CIRCLE;
  ((PRIM_ITEM_T *) (plt->data + size))->size = sizeof (PRIM_CIRCLE_T);
  size += sizeof (PRIM_ITEM_T);
  data = (plt->data + size);
  data->o = O;
  data->r = R;
  size += sizeof (PRIM_CIRCLE_T);
  plt->size = size;
  return plt;
}

PglPlot *
pri_arc (PglPlot * plt, pr_point O, pr_real R, pr_real A1, pr_real A2)
{
  int size;
  pr_real alp, bet;
  PRIM_ARC_T *data;
  alp = A1 - (pr_real) ((int) (A1 / (2. * M_PI))) * 2. * M_PI;
  bet = A2 - (pr_real) ((int) (A2 / (2. * M_PI))) * 2. * M_PI;
  if (alp < 0.)
    alp += 2. * M_PI;
  if (bet < 0.)
    bet += 2. * M_PI;
  size = ((PglPlot *) plt)->size;
  plt->data = g_realloc (plt->data,
			 size + sizeof (PRIM_ITEM_T) + sizeof (PRIM_ARC_T));
  ((PRIM_ITEM_T *) (plt->data + size))->type = PRIM_ARC;
  ((PRIM_ITEM_T *) (plt->data + size))->size = sizeof (PRIM_ARC_T);
  size += sizeof (PRIM_ITEM_T);
  data = (plt->data + size);
  data->o = O;
  data->r = R;
  data->alpha = alp;
  data->beta = bet;
  size += sizeof (PRIM_ARC_T);
  plt->size = size;
  return plt;
}

PglPlot *
pri_text (PglPlot * plt, pr_point O, pr_real S, pr_real A1,
	  const char *text, const char *family)
{
  int size, /* szi, */ slen, flen;
  pr_real alp;
  PRIM_TEXT_T *data;
  slen = strlen (text) + 1;
  flen = strlen (family) + 1;
  alp = A1 - (pr_real) ((int) (A1 / (2. * M_PI))) * 2. * M_PI;
  if (alp < 0.)
    alp += 2. * M_PI;
  size = ((PglPlot *) plt)->size;
  plt->data = g_realloc (plt->data,
			 size + sizeof (PRIM_ITEM_T) + sizeof (PRIM_TEXT_T) +
			 slen + flen);
  ((PRIM_ITEM_T *) (plt->data + size))->type = PRIM_TEXT;
  ((PRIM_ITEM_T *) (plt->data + size))->size =
    sizeof (PRIM_TEXT_T) + slen + flen;
  size += sizeof (PRIM_ITEM_T);
  data = (plt->data + size);
  data->o = O;
  data->s = S;
  data->alpha = alp;
  size += sizeof (PRIM_TEXT_T);
  data->text = plt->data + size;
  strcpy (data->text, text);
  size += slen;
  data->family = plt->data + size;
  strcpy (data->family, family);
  size += flen;
  plt->size = size;
  return plt;
}

PglPlot *
pri_sqr_bezier (PglPlot * plt, pr_point p0, pr_point p1, pr_point p2)
{
  int size;
  PRIM_SQR_BEZIER_T *data;
  size = ((PglPlot *) plt)->size;
  plt->data = g_realloc (plt->data,
			 size + sizeof (PRIM_ITEM_T) +
			 sizeof (PRIM_SQR_BEZIER_T));
  ((PRIM_ITEM_T *) (plt->data + size))->type = PRIM_SQR_BEZIER;
  ((PRIM_ITEM_T *) (plt->data + size))->size = sizeof (PRIM_SQR_BEZIER_T);
  size += sizeof (PRIM_ITEM_T);
  data = (plt->data + size);
  data->a = p0;
  data->b = p1;
  data->c = p2;
  size += sizeof (PRIM_SQR_BEZIER_T);
  plt->size = size;
  return plt;
}

PglPlot *
pri_cub_bezier (PglPlot * plt, pr_point p0, pr_point p1, pr_point p2,
		pr_point p3)
{
  int size;
  PRIM_CUB_BEZIER_T *data;
  size = ((PglPlot *) plt)->size;
  plt->data = g_realloc (plt->data,
			 size + sizeof (PRIM_ITEM_T) +
			 sizeof (PRIM_CUB_BEZIER_T));
  ((PRIM_ITEM_T *) (plt->data + size))->type = PRIM_CUB_BEZIER;
  ((PRIM_ITEM_T *) (plt->data + size))->size = sizeof (PRIM_CUB_BEZIER_T);
  size += sizeof (PRIM_ITEM_T);
  data = (plt->data + size);
  data->a = p0;
  data->b = p1;
  data->c = p2;
  data->d = p3;
  size += sizeof (PRIM_CUB_BEZIER_T);
  plt->size = size;
  return plt;
}

/*********************************************/
PglPlot *
pri_group_start (PglPlot * plt, PSI group_id)
{
  int size;
  PSI *data;
  size = plt->size;
  plt->data =
    g_realloc (plt->data, size + sizeof (PRIM_ITEM_T) + sizeof (PSI));
  ((PRIM_ITEM_T *) (plt->data + size))->type = PRIM_GROUP_START;
  ((PRIM_ITEM_T *) (plt->data + size))->size = sizeof (PSI);
  size += sizeof (PRIM_ITEM_T);
  data = (plt->data + size);
  *data = group_id;
  size += sizeof (PSI);
  plt->size = size;
  return plt;
}

PglPlot *
pri_group_end (PglPlot * plt, PSI group_id)
{
  int size;
  PSI *data;
  size = plt->size;
  plt->data =
    g_realloc (plt->data, size + sizeof (PRIM_ITEM_T) + sizeof (PSI));
  ((PRIM_ITEM_T *) (plt->data + size))->type = PRIM_GROUP_END;
  ((PRIM_ITEM_T *) (plt->data + size))->size = sizeof (PSI);
  size += sizeof (PRIM_ITEM_T);
  data = (plt->data + size);
  *data = group_id;
  size += sizeof (PSI);
  plt->size = size;
  return plt;
}

PglPlot *
pri_group_del (PglPlot * plt, PSI group_id)
{
  int size, start = 0, end = 0;
  size = plt->size;
  int position;
  short int mode;
  position = 0;
  while (size > position)
    {
      PSI str_size;
      mode = ((PRIM_ITEM_T *) (plt->data + position))->type;
      str_size = ((PRIM_ITEM_T *) (plt->data + position))->size;
      position += sizeof (PRIM_ITEM_T);
      switch (mode)
	{
	case PRIM_GROUP_START:
	  if (group_id == *(PSI *) (plt->data + position))
	    start = position - sizeof (PRIM_ITEM_T);
	  break;
	case PRIM_GROUP_END:
	  if (group_id == *(PSI *) (plt->data + position))
	    {
	      end = position + sizeof (PSI);
	      if (end > start)
		{
		  memmove (plt->data + start, plt->data + end, size - end);
		  plt->data = g_realloc (plt->data, size - (end - start));
		  size -= end - start;
		  position -= end - start;
		  start = end = 0;
		}
	    }
	  break;
	}
      position += str_size;
    }
  plt->size = size;
  return plt;
}

void
pri_clear (PglPlot * plt)
{
  if (plt->data)
    g_free (plt->data);
  plt->data = NULL;
  plt->size = 0;
}
