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
  plt = g_malloc0 (sizeof (PglPlot));
  plt->queue = g_array_new (FALSE, TRUE, sizeof (PRIM_ITEM_T));
  plt->w = w;
  plt->h = h;
  return plt;
}

PglPlot *
pri_line (PglPlot * plt, pr_point A, pr_point B)
{
  PRIM_LINE_T *data = g_malloc0 (sizeof (PRIM_LINE_T));
  PRIM_ITEM_T item = { PRIM_LINE, data };
  data->a = A;
  data->b = B;
  g_array_append_val (plt->queue, item);
  return plt;
}

PglPlot *
pri_rectangle (PglPlot * plt, pr_point A, pr_point B, PSI fill)
{
  PRIM_RECTANGLE_T *data = g_malloc0 (sizeof (PRIM_RECTANGLE_T));
  PRIM_ITEM_T item = { PRIM_RECTANGLE, data };
  data->a = A;
  data->b = B;
  data->fill = fill;
  g_array_append_val (plt->queue, item);
  return plt;
}

PglPlot *
pri_circle (PglPlot * plt, pr_point O, pr_real R)
{
  PRIM_CIRCLE_T *data = g_malloc0 (sizeof (PRIM_CIRCLE_T));
  PRIM_ITEM_T item = { PRIM_CIRCLE, data };
  data->o = O;
  data->r = R;
  g_array_append_val (plt->queue, item);
  return plt;
}

PglPlot *
pri_arc (PglPlot * plt, pr_point O, pr_real R, pr_real A1, pr_real A2)
{
  pr_real alp, bet;
  PRIM_ARC_T *data = g_malloc0 (sizeof (PRIM_ARC_T));
  PRIM_ITEM_T item = { PRIM_ARC, data };
  alp = A1 - (pr_real) ((int) (A1 / (2. * M_PI))) * 2. * M_PI;
  bet = A2 - (pr_real) ((int) (A2 / (2. * M_PI))) * 2. * M_PI;
  if (alp < 0.)
    alp += 2. * M_PI;
  if (bet < 0.)
    bet += 2. * M_PI;
  data->o = O;
  data->r = R;
  data->alpha = alp;
  data->beta = bet;
  g_array_append_val (plt->queue, item);
  return plt;
}

PglPlot *
pri_text (PglPlot * plt, pr_point O, pr_real S, pr_real A1,
	  const char *text, const char *family)
{
  int slen, flen, size = sizeof (PRIM_TEXT_T);
  pr_real alp;
  PRIM_TEXT_T *data;
  PRIM_ITEM_T item;
  slen = strlen (text) + 1;
  flen = strlen (family) + 1;
  data = g_malloc0 (size + slen + flen);
  item.type = PRIM_TEXT;
  item.data = data;
  alp = A1 - (pr_real) ((int) (A1 / (2. * M_PI))) * 2. * M_PI;
  if (alp < 0.)
    alp += 2. * M_PI;
  data->o = O;
  data->s = S;
  data->alpha = alp;
  data->text = ((void *) data) + size;
  strcpy (data->text, text);
  size += slen;
  data->family = ((void *) data) + size;
  strcpy (data->family, family);
  g_array_append_val (plt->queue, item);
  return plt;
}

PglPlot *
pri_sqr_bezier (PglPlot * plt, pr_point p0, pr_point p1, pr_point p2)
{
  PRIM_SQR_BEZIER_T *data = g_malloc0 (sizeof (PRIM_SQR_BEZIER_T));
  PRIM_ITEM_T item = { PRIM_SQR_BEZIER, data };
  data->a = p0;
  data->b = p1;
  data->c = p2;
  g_array_append_val (plt->queue, item);
  return plt;
}

PglPlot *
pri_cub_bezier (PglPlot * plt, pr_point p0, pr_point p1, pr_point p2,
		pr_point p3)
{
  PRIM_CUB_BEZIER_T *data = g_malloc0 (sizeof (PRIM_CUB_BEZIER_T));
  PRIM_ITEM_T item = { PRIM_CUB_BEZIER, data };
  data->a = p0;
  data->b = p1;
  data->c = p2;
  data->d = p3;
  g_array_append_val (plt->queue, item);
  return plt;
}

/*********************************************/
PglPlot *
pri_group_start (PglPlot * plt, PSI group_id)
{
  PSI *data = g_malloc0 (sizeof (PSI));
  PRIM_ITEM_T item = { PRIM_GROUP_START, data };
  *data = group_id;
  g_array_append_val (plt->queue, item);
  return plt;
}

PglPlot *
pri_group_end (PglPlot * plt, PSI group_id)
{
  PSI *data = g_malloc0 (sizeof (PSI));
  PRIM_ITEM_T item = { PRIM_GROUP_END, data };
  *data = group_id;
  g_array_append_val (plt->queue, item);
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
  int i;
  GArray *q = plt->queue;
  for (i = q->len - 1; i >= 0; i++)
    g_free (g_array_index (q, PRIM_ITEM_T, i).data);
  g_array_remove_range (q, 0, q->len);
}
