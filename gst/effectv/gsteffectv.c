/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 *
 * EffecTV:
 * Copyright (C) 2001 FUKUCHI Kentarou
 *
 * EffecTV is free software. This library is free software;
 * you can redistribute it and/or
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gsteffectv.h"
#include "gstaging.h"
#include "gstdice.h"
#include "gstedge.h"
#include "gstquark.h"
#include "gstrev.h"
#include "gstshagadelic.h"
#include "gstvertigo.h"
#include "gstwarp.h"

struct _elements_entry
{
  const gchar *name;
    GType (*type) (void);
};

static const struct _elements_entry _elements[] = {
  {"edgetv", gst_edgetv_get_type},
  {"agingtv", gst_agingtv_get_type},
  {"dicetv", gst_dicetv_get_type},
  {"warptv", gst_warptv_get_type},
  {"shagadelictv", gst_shagadelictv_get_type},
  {"vertigotv", gst_vertigotv_get_type},
  {"revtv", gst_revtv_get_type},
  {"quarktv", gst_quarktv_get_type},
  {NULL, 0},
};

static gboolean
plugin_init (GstPlugin * plugin)
{
  gint i = 0;

  while (_elements[i].name) {
    if (!gst_element_register (plugin, _elements[i].name,
            GST_RANK_NONE, (_elements[i].type) ()))
      return FALSE;
    i++;
  }

  return TRUE;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "effectv",
    "effect plugins from the effectv project",
    plugin_init, VERSION, "LGPL", GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN);
