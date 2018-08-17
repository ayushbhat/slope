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

#include "slope/text.h"

SlopeText* slope_text_new (const char *font_desc)
{
    SlopeText *self = g_new (SlopeText, 1);

    self->font_desc = pango_font_description_from_string (font_desc);
    self->layout = NULL;
    self->cr = NULL;
    return self;
}


void slope_text_init (SlopeText *self, cairo_t *cr)
{
    if (self->layout) g_object_unref (self->layout);
    self->layout = pango_cairo_create_layout (cr);
    pango_layout_set_font_description (self->layout, self->font_desc);
    self->cr = cr;
}


void slope_text_delete (SlopeText *self)
{
    if (self->layout) g_object_unref (self->layout);
    pango_font_description_free (self->font_desc);
    g_free (self);
}


int slope_text_set (SlopeText *self, const char *text)
{
    pango_layout_set_text (self->layout, text, -1);
    return 0;
}


int slope_text_show (SlopeText *self) {
    pango_cairo_show_layout (self->cr, self->layout);
    return 0;
}


int slope_text_get_size (SlopeText *self, int *width, int *height)
{
    pango_layout_get_pixel_size(self->layout, width, height);
    return 0;
}

/* slope/text.c */