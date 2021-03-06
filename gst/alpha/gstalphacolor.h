/* GStreamer alphacolor element
 * Copyright (C) 2005 Wim Taymans <wim@fluendo.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _GST_ALPHA_COLOR_H_
#define _GST_ALPHA_COLOR_H_

#include <gst/base/gstbasetransform.h>

#define GST_TYPE_ALPHA_COLOR \
  (gst_alpha_color_get_type())
#define GST_ALPHA_COLOR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_ALPHA_COLOR,GstAlphaColor))
#define GST_ALPHA_COLOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_ALPHA_COLOR,GstAlphaColorClass))
#define GST_IS_ALPHA_COLOR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_ALPHA_COLOR))
#define GST_IS_ALPHA_COLOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_ALPHA_COLOR))

typedef struct _GstAlphaColor GstAlphaColor;
typedef struct _GstAlphaColorClass GstAlphaColorClass;

struct _GstAlphaColor
{
  GstBaseTransform element;

  /*< private >*/
  /* caps */
  gint in_width, in_height;
  gboolean in_rgba;
  gint out_width, out_height;
};

struct _GstAlphaColorClass
{
  GstBaseTransformClass parent_class;
};

GType   gst_alpha_color_get_type (void);

#endif /* _GST_ALPHA_COLOR_H_ */
