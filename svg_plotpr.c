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
prp_line_svg (FILE * svgf, pr_scale psc, PrimBuf prb)
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
prp_circle_svg (FILE * svgf, pr_scale psc, PrimBuf prb)
{
  pr_point O, A;
  pr_real R;
  PRIM_CIRCLE_T *data;
  data = (PRIM_CIRCLE_T *) (prb);
  A.x = O.x - R;
  A.y = O.y + R;
}

void
prp_arc_svg (FILE * svgf, pr_scale psc, PrimBuf prb)
{
  int r;
  pr_point O, A;
  pr_real R, A1, A2;
  PRIM_ARC_T *data;
  data = (PRIM_ARC_T *) (prb);
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
prp_sqr_bezier_svg (FILE * svgf, pr_scale psc, PrimBuf prb)
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

typedef struct
{
  int pos;
  char used;
} POTPATH;
POTPATH *
extract_potential_path (PrimBuf prb, int *npot)
{
  int position, len;
  PSI mode, str_size;
  len = *(int *) prb;
  int Nres;
  POTPATH *res;
  Nres = 0;
  res = 0;
  position = sizeof (int);
  while (len > position)
    {
      mode = *(PSI *) (prb + position);
      position += sizeof (PSI);
      str_size = *(PSI *) (prb + position);
      position += sizeof (PSI);
      switch (mode)
	{
	case PRIM_LINE:
	  res = realloc (res, sizeof (POTPATH) * (1 + Nres));
	  res[Nres].pos = position - sizeof (PSI);
	  res[Nres].used = 0;
	  Nres++;
	  break;
	case PRIM_POINT:
	  position += sizeof (PRIM_POINT_T);
	  break;
	case PRIM_CIRCLE:
	  break;
	case PRIM_ARC:
	  break;
	case PRIM_SQR_BEZIER:
	  res = realloc (res, sizeof (POTPATH) * (1 + Nres));
	  res[Nres].pos = position - sizeof (PSI);
	  res[Nres].used = 0;
	  Nres++;
	  break;
	}
      position += str_size;
    }
  *npot = Nres;
  return res;
}

typedef struct
{
  int pos;
  char rev;
} PATHELEM;
double
close_point (pr_point a, pr_point b)
{
  double res = 0., delta;
  delta = a.x - b.x;
  res += delta * delta;
  delta = a.y - b.y;
  return res + delta * delta;
}

int
is_the_elem (PrimBuf prb, int pos, pr_point the_pt, pr_point * oposit)
{
  PSI mode;
  PRIM_LINE_T *pPrl;
  PRIM_SQR_BEZIER_T *pPrQb;
  pr_point start, end;
  int res;
  mode = *(PSI *) (prb + pos);
  switch (mode)
    {
    case PRIM_LINE:
      pPrl = (prb + pos + sizeof (PSI));
      start = pPrl->a;
      end = pPrl->b;
      break;
    case PRIM_SQR_BEZIER:
      pPrQb = (prb + pos + sizeof (PSI));
      start = pPrQb->a;
      end = pPrQb->c;
      break;
    }
  res = 0;
  if (close_point (start, the_pt) < 1.e-18)
    {
      res = 1;
      *oposit = end;
    }
  if (close_point (end, the_pt) < 1.e-18)
    {
      res = 2;
      *oposit = start;
    }
  return res;
}

PATHELEM *
search_path (PrimBuf prb, POTPATH * pot, int npot, int *lenpath)
{
  pr_point start, end;
  PATHELEM *res;
  PRIM_LINE_T *pPrl;
  PRIM_SQR_BEZIER_T *pPrQb;
  PSI mode;
  int nres, i;
  for (i = 0; i < npot; i++)
    if (!pot[i].used)
      break;
  if (i == npot)
    return NULL;
  res = malloc (sizeof (PATHELEM));
  nres = 1;
  pot[i].used = 1;
  mode = *(PSI *) (prb + pot[i].pos - sizeof (PSI));
  switch (mode)
    {
    case PRIM_LINE:
      pPrl = (prb + pot[i].pos + sizeof (PSI));
      start = pPrl->a;
      end = pPrl->b;
      break;
    case PRIM_SQR_BEZIER:
      pPrQb = (prb + pot[i].pos + sizeof (PSI));
      start = pPrQb->a;
      end = pPrQb->c;
      break;
    }
  res[0].pos = pot[i].pos;
  res[0].rev = 0;
  while (1)
    {
      pr_point opos;
      int is;
      for (i = 0; i < npot; i++)
	if (pot[i].used == 0
	    && (is = is_the_elem (prb, pot[i].pos, end, &opos)))
	  {
	    pot[i].used = 1;
	    res = realloc (res, sizeof (PATHELEM) * (1 + nres));
	    res[nres].pos = pot[i].pos;
	    res[nres].rev = is == 2 ? 1 : 0;
	    end = opos;
	    nres++;
	    break;
	  }
      if (i == npot)
	break;
    }
  while (1)
    {
      pr_point opos;
      int is;
      for (i = 0; i < npot; i++)
	if (pot[i].used == 0
	    && (is = is_the_elem (prb, pot[i].pos, start, &opos)))
	  {
	    pot[i].used = 1;
	    res = realloc (res, sizeof (PATHELEM) * (1 + nres));
	    memmove (res + 1, res, sizeof (PATHELEM) * nres);
	    res[0].pos = pot[i].pos;
	    res[0].rev = (is == 1) ? 1 : 0;
	    start = opos;
	    nres++;
	    break;
	  }
      if (i == npot)
	break;
    }
  *lenpath = nres;
  return res;
}

typedef struct
{
  int nelem;
  PATHELEM *path;
} PATH;
typedef struct
{
  int npath;
  PATH *pathes;
} PATHES;
PATHES
search_pathes (PrimBuf prb)
{
  POTPATH *pot;
  int npot, lenpath;
  PATHELEM *path;
  PATHES res;
  res.pathes = NULL;
  res.npath = 0;
  pot = extract_potential_path (prb, &npot);
  while ((path = search_path (prb, pot, npot, &lenpath)) != NULL)
    {
      res.pathes = realloc (res.pathes, sizeof (PATH) * (1 + res.npath));
      res.pathes[res.npath].path = path;
      res.pathes[res.npath].nelem = lenpath;
      res.npath++;
    }
  free (pot);
  return res;
}

void
del_pathes (PATHES pathes)
{
  int i;
  for (i = 0; i < pathes.npath; i++)
    free (pathes.pathes[i].path);
  free (pathes.pathes);
}

char *
path_2_string (PATH path, PglPlot * prb, pr_scale psc)
{
  PSI mode;
  int pos, len, lenout, i;
  pr_point pt1, pt2;
  char buf[80], *out;
  PRIM_LINE_T *pPrl;
  PRIM_SQR_BEZIER_T *pPrQb;
  pos = path.path[0].pos;
  mode = *(PSI *) (prb + pos);
  switch (mode)
    {
    case PRIM_LINE:
      pPrl = (prb + pos + sizeof (PSI));
      if (path.path[0].rev)
	pt1 = prp_2svg (psc, pPrl->b);
      else
	pt1 = prp_2svg (psc, pPrl->a);
      break;
    case PRIM_SQR_BEZIER:
      pPrQb = (prb + pos + sizeof (PSI));
      if (path.path[0].rev)
	pt1 = prp_2svg (psc, pPrQb->c);
      else
	pt1 = prp_2svg (psc, pPrQb->a);
      break;
    }
  lenout = 1;
  len = sprintf (buf, "M%lg %lg", pt1.x, pt1.y);
  out = malloc (lenout + len);
  strcpy (out + (lenout - 1), buf);
  lenout += len;
  for (i = 0; i < path.nelem; i++)
    {
      pos = path.path[i].pos;
      mode = *(PSI *) (prb + pos);
      switch (mode)
	{
	case PRIM_LINE:
	  pPrl = (prb + pos + sizeof (PSI));
	  if (path.path[i].rev)
	    {
	      pt1 = prp_2svg (psc, pPrl->a);
	    }
	  else
	    {
	      pt1 = prp_2svg (psc, pPrl->b);
	    }
	  len = sprintf (buf, "L%lg %lg", pt1.x, pt1.y);
	  out = realloc (out, lenout + len);
	  strcpy (out + (lenout - 1), buf);
	  lenout += len;
	  break;
	case PRIM_SQR_BEZIER:
	  pPrQb = (prb + pos + sizeof (PSI));
	  pt2 = prp_2svg (psc, pPrQb->b);
	  if (path.path[i].rev)
	    pt1 = prp_2svg (psc, pPrQb->a);
	  else
	    pt1 = prp_2svg (psc, pPrQb->c);
	  len = sprintf (buf, "Q%lg %lg %lg %lg", pt2.x, pt2.y, pt1.x, pt1.y);
	  out = realloc (out, lenout + len);
	  strcpy (out + (lenout - 1), buf);
	  lenout += len;
	  break;
	}
    }
  return out;
}

void
prp_step_by_step_svg (FILE * svgf, pr_scale psc, PglPlot * prb)
{
  int position, len;
  PSI mode;
  len = *(int *) prb;
  position = sizeof (int);
  setlocale (LC_NUMERIC, "C");
  {
    PATHES pathes;
    int i;
    pathes = search_pathes (prb);
    for (i = 0; i < pathes.npath; i++)
      {
	char *cp;
	cp = path_2_string (pathes.pathes[i], prb, psc);
	fprintf (svgf, "<path d=\"%s\"/>\n", cp);
	free (cp);
      }
    del_pathes (pathes);
  }
  setlocale (LC_NUMERIC, "");
}
