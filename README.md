# pgl
__Primitives graphics library__

Simple 2d graphics library intended to unify process of creating vector graphics files and plotting on screen.
## Installation

* Make your build directory. E.g. <code>mkdir build</code> in folder with sources.
* Configure with CMake: <code>cmake ..</code> (replace <<..>> with path to sources.
* Do <code>make install</code> if you are using UNIX system or something like in other systems.

# API

<code>PglPlot *pri_init (pr_real w, pr_real h);</code>

Creates buffer for primitives

<code>PglPlot *pri_line (PglPlot *, pr_point A, pr_point B, pr_color clr);</code>

Draws line from A to B

<code>PglPlot *pri_rectangle (PglPlot *, pr_point A, pr_point B, PSI fill, pr_color clr);</code>

Draws rectangle vith diagonal from A to B filled by clr

<code>PglPlot *pri_circle (PglPlot *, pr_point O, pr_real R, pr_color clr);</code>

Draws circle with center in point O and radius R

<code>PglPlot *pri_arc (PglPlot *, pr_point O, pr_real R, pr_real A1, pr_real A2, pr_color clr);</code>

draws an arc

<code>PglPlot *pri_sqr_bezier (PglPlot *, pr_point p0, pr_point p1, pr_point p2, pr_color clr);</code>

draws square bezier

<code>PglPlot *pri_cub_bezier (PglPlot *, pr_point p0, pr_point p1, pr_point p2, pr_point p3, pr_color clr);</code>

draws cubic bezier

<code>PglPlot *pri_group_start (PglPlot *, PSI group_id);</code>

Styarts a group of primitives with identefier *group_id*

<code>PglPlot *pri_group_end (PglPlot *, PSI group_id);</code>

ends the group of primitives with identefier *group_id*

<code>PglPlot *pri_group_del (PglPlot *, PSI group_id);</code>

Destroy the group of primitives with identefier *group_id*

<code>PglPlot *pri_text (PglPlot * plt, pr_point O, pr_real S, pr_real A1, const char *text, const char *family, pr_color clr);</code>

Write some text (not implemented).

<code>void pri_clear (PglPlot * plt);</code>

Remove all primitives in *plt*

<code>void prp_step_by_step_ps (FILE * psf, pr_scale psc, PglPlot *);</code>

Write primitives as postscript

<code>void prp_step_by_step_eps (FILE * psf, pr_scale psc, PglPlot *);</code>

(not implemented)

<code>void prp_step_by_step_svg (FILE * svgf, pr_scale psc, PglPlot *);</code>

(not implemented)

