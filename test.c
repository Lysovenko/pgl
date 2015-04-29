/*  test.c
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

#include <stdlib.h>
#include <gtk/gtk.h>
#include <math.h>
#include <pgl.h>
static PrimBuf ext_prb;
static gboolean
expose_event (GtkWidget * widget, GdkEventExpose * event, gpointer * data)
{
  GdkGC *gc;
  gc = gdk_gc_new (widget->window);
  prp_step_by_step_gdk (widget->window, gc, ext_prb);
  g_object_unref (gc);
  return TRUE;
}

static gint
Delete (GtkWidget * widget, gpointer * data)
{
  gtk_main_quit ();
  return FALSE;
}

void
CreateDraw ()
{
  GtkWidget *window;
  GtkWidget *vBox;
  GtkWidget *DrawArea;
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Test the PGL library");
  vBox = gtk_vbox_new (FALSE, 0);
  DrawArea = gtk_drawing_area_new ();
  gtk_widget_set_size_request (DrawArea, 100, 100);
  gtk_container_add (GTK_CONTAINER (window), vBox);
  gtk_box_pack_start (GTK_BOX (vBox), DrawArea, TRUE, TRUE, 0);
  g_signal_connect (GTK_OBJECT (window), "delete-event",
		    GTK_SIGNAL_FUNC (Delete), NULL);
  g_signal_connect (GTK_OBJECT (DrawArea), "expose-event",
		    GTK_SIGNAL_FUNC (expose_event), NULL);
  gtk_widget_show (DrawArea);
  /* Ask to receive events the drawing area doesn't normally
   * subscribe to
   */
  gtk_widget_set_events (DrawArea, gtk_widget_get_events (DrawArea)
			 | GDK_LEAVE_NOTIFY_MASK
			 | GDK_BUTTON_PRESS_MASK
			 | GDK_POINTER_MOTION_MASK
			 | GDK_POINTER_MOTION_HINT_MASK);
  gtk_widget_show (vBox);
  gtk_widget_show (window);
  /* TEST */
  ext_prb = pri_init (2., 2.);
  {
    pr_point A = { 0., 0. }, B =
    {
    1., 2.}, C =
    {
    2., 0.}, D =
    {
    1., 1.};
    pri_group_start (ext_prb, 555);
    pri_rectangle (ext_prb, (pr_point)
		   {
		   1.5, 1.5}, D, TRUE, 0x0000FF);
    pri_text (ext_prb, D, 9, 0., "1xt", "Arial", 0xFF00AA);
    pri_group_end (ext_prb, 555);
    pri_sqr_bezier (ext_prb, A, B, C, 0x0F00FF);
    pri_cub_bezier (ext_prb, A, B, A, C, 0x0F00FF);
    pri_line (ext_prb, A, B, 0xFF0000);
    pri_line (ext_prb, A, D, 0x0F0000);
    pri_line (ext_prb, B, C, 0xFF0000);
    pri_rectangle (ext_prb, A, B, 0, 0x00C0A0);
  }
}

int
main (int argc, char **argv)
{
  gtk_init (&argc, &argv);
  CreateDraw ();
  gtk_main ();
  {
    FILE *fp = stdout;		/* fopen("test.eps", "w"); */
    pr_scale psc = { 0., 0., 300., 10 };
    prp_step_by_step_ps (fp, psc, ext_prb);
    /* fclose(fp); */
  }
  g_print ("%g\n", 3.14159);
  return 0;
}
