/*  primitives.h
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

/**
Creating primitives*/
#ifndef __PRIMITIVES_H__
#define __PRIMITIVES_H__
#include "pgl.h"
enum PRIMITIVES
{
  PRIM_POINT = 0,
  PRIM_LINE,
  PRIM_RECTANGLE,
  PRIM_CIRCLE,
  PRIM_ARC,
  PRIM_TEXT,
  PRIM_SQR_BEZIER,
  PRIM_GROUP_START,
  PRIM_GROUP_END,
  PRIMITIVES_SIZE
};
typedef struct
{
  PSI type, size;
} PRIM_ITEM_T;
typedef struct
{
  pr_point a, b;
  pr_color clr;
} PRIM_LINE_T;
typedef struct
{
  pr_point a, b;
  PSI fill;
  pr_color clr;
} PRIM_RECTANGLE_T;
typedef struct
{
  pr_point a;
  pr_color clr;
} PRIM_POINT_T;
typedef struct
{
  pr_point o;
  pr_real r;
  pr_color clr;
} PRIM_CIRCLE_T;
typedef struct
{
  pr_point o;
  pr_real r, alpha, beta;
  pr_color clr;
} PRIM_ARC_T;
typedef struct
{
  pr_point o;
  pr_real s, alpha;
  char *text, *family;
  pr_color clr;
} PRIM_TEXT_T;
typedef struct
{
  pr_point a, b, c;
  pr_color clr;
} PRIM_SQR_BEZIER_T;
#endif
