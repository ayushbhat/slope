/*
 * Copyright (C) 2018  Elvis Teixeira
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

#include "slope/figure_p.h"
#include "slope/item_p.h"

G_DEFINE_TYPE_WITH_PRIVATE(SlopeFigure, slope_figure, G_TYPE_OBJECT)


/* signals */
enum {
    SIG_ADD,
    SIG_REMOVE,
    SIG_CHANGED,
    SIG_LAST
};
static guint figure_signals[SIG_LAST] = { 0 };


/* properties */
enum {
  PROP_0,
  PROP_BG_FILL_COLOR,
  PROP_BG_STROKE_COLOR,
  PROP_BG_STROKE_WIDTH,
  PROP_LAST
};
static GParamSpec *figure_props[PROP_LAST] = { NULL };


/* local decls */
static void slope_figure_finalize (GObject *self);
static void slope_figure_dispose (GObject *self);
static void base_figure_draw (SlopeFigure *self, cairo_t *cr, int width, int height);
static void slope_figure_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void slope_figure_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void base_figure_set_root_item (SlopeFigure *self, SlopeItem *item);


static void
slope_figure_class_init (SlopeFigureClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->dispose = slope_figure_dispose;
    gobject_class->finalize = slope_figure_finalize;
    gobject_class->set_property = slope_figure_set_property;
    gobject_class->get_property = slope_figure_get_property;

    klass->draw = base_figure_draw;
    klass->set_root_item = base_figure_set_root_item;

    figure_props[PROP_BG_FILL_COLOR] =
          g_param_spec_uint ("bg-fill-color",
                             "Background fill color",
                             "Specify the background fill color",
                             SLOPE_COLOR_NULL, SLOPE_WHITE, SLOPE_WHITE,
                             G_PARAM_READWRITE|G_PARAM_EXPLICIT_NOTIFY);

    figure_props[PROP_BG_STROKE_COLOR] =
          g_param_spec_uint ("bg-stroke-color",
                             "Background stroke color",
                             "Specify the background stroke color",
                             SLOPE_COLOR_NULL, SLOPE_WHITE, SLOPE_COLOR_NULL,
                             G_PARAM_READWRITE|G_PARAM_EXPLICIT_NOTIFY);

    figure_props[PROP_BG_STROKE_WIDTH] =
          g_param_spec_double ("bg-stroke-width",
                               "Background stroke width",
                               "Specify the background stroke width",
                               0.0, 8.0, 2.0,
                               G_PARAM_READWRITE|G_PARAM_EXPLICIT_NOTIFY);

    g_object_class_install_properties (gobject_class, PROP_LAST, figure_props);


    figure_signals[SIG_ADD] =
        g_signal_new ("add",
                      G_OBJECT_CLASS_TYPE (gobject_class),
                      G_SIGNAL_RUN_FIRST,
                      G_STRUCT_OFFSET (SlopeFigureClass, set_root_item),
                      NULL, NULL,
                      NULL,
                      G_TYPE_NONE, 0);

    figure_signals[SIG_CHANGED] =
        g_signal_new ("changed",
                      G_OBJECT_CLASS_TYPE (gobject_class),
                      G_SIGNAL_RUN_FIRST,
                      0,
                      NULL, NULL,
                      NULL,
                      G_TYPE_NONE,
                      0);
}


static void
slope_figure_init (SlopeFigure *self)
{
    SlopeFigurePrivate *m = SLOPE_FIGURE_GET_PRIVATE (self);

    m->bg_fill_color = SLOPE_WHITE;
    m->bg_stroke_color = SLOPE_COLOR_NULL;
    m->options = RoundedRect;
    m->bg_stroke_width = 2.0;
    m->item_tree = NULL;
    m->text = slope_text_new ("Monospace 9");
    m->title = g_strdup("Slope");
    m->title_color = SLOPE_BLACK;
}


static void
slope_figure_set_property (GObject *object, guint prop_id,
                           const GValue *value, GParamSpec *pspec)
{
    SlopeFigurePrivate *m = SLOPE_FIGURE_GET_PRIVATE(object);

    switch (prop_id) {
        case PROP_BG_FILL_COLOR:
            m->bg_fill_color = g_value_get_uint(value);
            break;
        case PROP_BG_STROKE_COLOR:
            m->bg_stroke_color = g_value_get_uint(value);
            break;
        case PROP_BG_STROKE_WIDTH:
            m->bg_stroke_width = g_value_get_double(value);
            break;
    }
}


static void
slope_figure_get_property (GObject *object, guint prop_id,
                           GValue *value, GParamSpec *pspec)
{
    SlopeFigurePrivate *m = SLOPE_FIGURE_GET_PRIVATE(object);

    switch (prop_id) {
        case PROP_BG_FILL_COLOR:
            g_value_set_uint(value, m->bg_fill_color);
            break;
        case PROP_BG_STROKE_COLOR:
            g_value_set_uint(value, m->bg_stroke_color);
            break;
        case PROP_BG_STROKE_WIDTH:
            g_value_set_double(value, m->bg_stroke_width);
            break;
    }
}


gpointer
item_cleanup (gpointer data, gpointer context)
{
    SlopeItem *item = SLOPE_ITEM_PRIVATE (data)->publ_obj;
    SLOPE_UNUSED(context);

    g_object_unref (G_OBJECT (item));
    return NULL;
}


static void
slope_figure_dispose (GObject *object)
{
    SlopeFigure *self = SLOPE_FIGURE (object);
    SlopeFigurePrivate *m = SLOPE_FIGURE_GET_PRIVATE (self);

    slope_tree_destroy (SLOPE_TREE (m->item_tree), item_cleanup);
    slope_text_delete (m->text);

    G_OBJECT_CLASS (slope_figure_parent_class)->dispose (object);
}


static void
slope_figure_finalize (GObject *object)
{
    SlopeFigure *self = SLOPE_FIGURE (object);
    SlopeFigurePrivate *m = SLOPE_FIGURE_GET_PRIVATE (self);

    // TODO

    G_OBJECT_CLASS (slope_figure_parent_class)->finalize (object);
}


SlopeFigure* /* public new() function*/
slope_figure_new (void)
{
    return SLOPE_FIGURE (g_object_new (SLOPE_TYPE_FIGURE, NULL));
}


static void
figure_draw_rect (SlopeFigure *self, cairo_t *cr, SlopeRect *rect)
{
    SlopeFigurePrivate *m = SLOPE_FIGURE_GET_PRIVATE (self);

    if (slope_enabled(m->options, RoundedRect)) {
        /* translate cr to account for the area loss */
        rect->width -= 20;
        rect->height -= 20;
        cairo_translate (cr, 10.0, 10.0);
        slope_cairo_round_rect (cr, rect, 10);
    } else {
        slope_cairo_rect (cr, rect);
    }

    slope_cairo_draw (
          cr, m->bg_stroke_width,
          m->bg_fill_color, m->bg_stroke_color);
}


static void
figure_draw_items (SlopeFigure *self, SlopeTree *node,
                   cairo_t *cr, const SlopeRect *rect)
{
    /* Do this node's drawing */
    draw_item_p ((SlopeItemPrivate *) node, cr, rect);

    /* Recurse to the subtrees */
    node = node->first;
    while (node != NULL) {
        figure_draw_items(self, node, cr, rect);
        node = node->next;
    }
}


static void
base_figure_draw (SlopeFigure *self, cairo_t *cr, int width, int height)
{
    SlopeFigurePrivate *m = SLOPE_FIGURE_GET_PRIVATE (self);
    SlopeRect rect;

    cairo_save (cr);
    slope_text_init (m->text, cr);

    rect.x = 0.0;
    rect.y = 0.0;
    rect.width = (double) width;
    rect.height = (double) height;

    if (slope_rgba_is_visible(m->bg_fill_color) ||
            slope_rgba_is_visible(m->bg_stroke_color)) {
        figure_draw_rect (self, cr, &rect);
    }

    if (m->item_tree != NULL) {
        figure_draw_items(self, SLOPE_TREE (m->item_tree), cr, &rect);
    }

    /* Draw the title, if is is visible no the current background color */
    if (m->title != NULL &&
            slope_rgba_is_visible(m->title_color) &&
            (m->title_color != m->bg_fill_color)) {
        SlopeRect ink, logical;
        slope_text_set (m->text, m->title);
        slope_text_get_extents (m->text, &ink, &logical);
        slope_cairo_set_rgba (cr, m->title_color);
        cairo_move_to (cr, (rect.width - logical.width) / 2.0, 10.0);
        slope_text_show (m->text);
    }

    /* Give the cairo context back to the user in
     * the same state in which we received it */
    cairo_restore (cr);
}


void /* public draw() function*/
slope_figure_draw (SlopeFigure *self, cairo_t *cr, int width, int height)
{
    g_return_if_fail (SLOPE_IS_FIGURE (self));
    SLOPE_FIGURE_GET_CLASS (self)->draw(self, cr, width, height);
}


int /* public save() function*/
slope_figure_save (SlopeFigure *self, const gchar *file_name,
                   int width, int height, const gchar *format)
{
    cairo_surface_t *surf;
    cairo_t *cr;

    g_return_val_if_fail (SLOPE_IS_FIGURE (self), -1);
    g_return_val_if_fail (file_name, -1);
    g_return_val_if_fail (width > 0 && height > 0, -1);
    g_return_val_if_fail (format, -1);

    if (g_strcmp0(format, "png") == 0) {
        surf = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
        cr = cairo_create (surf);
        slope_figure_draw (self, cr, width, height);
        cairo_surface_write_to_png (surf, file_name);
    } else {
        return -2;
    }

    cairo_surface_destroy (surf);
    cairo_destroy (cr);
    return 0;
}


void
slope_figure_set_title (SlopeFigure *self, const gchar *title)
{
    SlopeFigurePrivate *m;

    g_return_if_fail(SLOPE_IS_FIGURE(self));
    m = SLOPE_FIGURE_GET_PRIVATE (self);

    if (m->title)
        g_free(m->title);

    m->title = NULL;
    if (title)
        m->title = g_strdup(title);
}


const gchar*
slope_figure_get_title (SlopeFigure *self)
{
    g_return_val_if_fail(SLOPE_IS_FIGURE(self), NULL);
    return SLOPE_FIGURE_GET_PRIVATE (self)->title;
}


static void
base_figure_set_root_item (SlopeFigure *self, SlopeItem *item)
{
    SlopeItemPrivate *item_p;
    SlopeFigurePrivate *fig_p;
    SlopeItemClass *item_class;

    fig_p = SLOPE_FIGURE_GET_PRIVATE (self);
    item_p = SLOPE_ITEM_GET_PRIVATE (item);
    item_class = SLOPE_ITEM_GET_CLASS (item);

    slope_tree_destroy (SLOPE_TREE (fig_p->item_tree), item_cleanup);

    item_p->figure = self;
    fig_p->item_tree = SLOPE_TREE (item_p);

    if (item_class->added) {
        item_class->added (item, NULL, self);
    }
}


void slope_figure_set_root_item (SlopeFigure *self, SlopeItem *item) {
    g_return_if_fail (SLOPE_IS_FIGURE (self));
    g_return_if_fail (SLOPE_IS_ITEM (item));
    SLOPE_FIGURE_GET_CLASS (self)->set_root_item (self, item);
}

/* slope/figure.c */
