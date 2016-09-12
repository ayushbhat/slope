/*
 * Copyright (C) 2016  Elvis Teixeira
 *
 * This source code is free software: you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General
 * Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any
 * later version.
 *
 * This source code is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SLOPE_FIGURE_H
#define SLOPE_FIGURE_H

#include <glib-object.h>
#include <slope/drawing.h>

#define SLOPE_FIGURE_TYPE              (slope_figure_get_type())
#define SLOPE_FIGURE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), SLOPE_FIGURE_TYPE, SlopeFigure))
#define SLOPE_FIGURE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), SLOPE_FIGURE_TYPE, SlopeFigureClass))
#define SLOPE_FIGURE_GET_CLASS(obj)    (SLOPE_FIGURE_CLASS(G_OBJECT_GET_CLASS(obj)))
#define SLOPE_IS_FIGURE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), SLOPE_FIGURE_TYPE))
#define SLOPE_IS_FIGURE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE((klass), SLOPE_FIGURE_TYPE))

SLOPE_BEGIN_DECLS

struct _SlopeFigure
{
  GObject parent;

  /* Padding to allow adding up to 4 members
     without breaking ABI. */
  gpointer padding[4];
};


typedef struct
_SlopeFigureClass
{
  GObjectClass parent_class;

  void (*draw) (SlopeFigure *self, const SlopeRect *rect, cairo_t *cr);
  void (*add_item) (SlopeFigure *self, SlopeItem *item);

  /* Padding to allow adding up to 4 members
     without breaking ABI. */
  gpointer padding[4];
}
SlopeFigureClass;


GType slope_figure_get_type(void) G_GNUC_CONST;

SlopeFigure* slope_figure_new (void);

GList* slope_figure_get_item_list (SlopeFigure *self);

void slope_figure_add_item (SlopeFigure *self, SlopeItem *item);

SlopeColor slope_figure_get_background_color (SlopeFigure *self);

void slope_figure_set_background_color (SlopeFigure *self, SlopeColor color);

void slope_figure_draw (SlopeFigure *self, const SlopeRect *rect, cairo_t *cr);

void slope_figure_write_to_png (SlopeFigure *self, const char *filename, int width, int height);

SlopeView* slope_figure_get_view (SlopeFigure *self);

SLOPE_END_DECLS

#endif /* SLOPE_FIGURE_H */