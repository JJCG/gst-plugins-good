/*
 * gstcmmlutils.c - GStreamer CMML utility functions
 * Copyright (C) 2005 Alessandro Decina
 * 
 * Authors:
 *   Alessandro Decina <alessandro@nnva.org>
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

#include "gstcmmlutils.h"

#include <math.h>
#include <string.h>

typedef struct
{
  GList *clips;
  gpointer user_data;
} GstCmmlTrack;

GstClockTime
gst_cmml_clock_time_from_npt (const gchar * time)
{
  GstClockTime res;
  gint fields;
  gint hours = 0;
  gint minutes = 0;
  gint seconds = 0;
  gint mseconds = 0;
  GstClockTime hours_t = 0, seconds_t = 0;

  if (!strncmp (time, "npt:", 4))
    time += 4;

  /* parse npt-hhmmss */
  fields = sscanf (time, "%d:%d:%d.%d", &hours, &minutes, &seconds, &mseconds);
  if (fields == 4) {
    if (hours < 0 || (guint) minutes > 59 || (guint) seconds > 59)
      goto bad_input;

    hours_t = gst_util_uint64_scale (hours, GST_SECOND * 3600, 1);
    if (hours_t == G_MAXUINT64)
      goto overflow;

    seconds_t = seconds * GST_SECOND;
  } else {
    /* parse npt-sec */
    hours_t = 0;
    minutes = 0;
    fields = sscanf (time, "%d.%d", &seconds, &mseconds);
    if (seconds < 0)
      goto bad_input;

    seconds_t = gst_util_uint64_scale (seconds, GST_SECOND, 1);
    if (seconds == G_MAXUINT64)
      goto overflow;
  }

  if ((guint) mseconds > 999)
    goto bad_input;

  res = (minutes * 60) * GST_SECOND + mseconds * GST_MSECOND;
  if (G_MAXUINT64 - hours_t - seconds_t < res)
    goto overflow;

  res += hours_t + seconds_t;

  return res;

bad_input:
overflow:
  return GST_CLOCK_TIME_NONE;
}

GstClockTime
gst_cmml_clock_time_from_smpte (const gchar * time)
{
  GstClockTime res;
  GstClockTime hours_t;
  gint hours, minutes, seconds;
  gdouble framerate;
  gfloat frames;
  gint fields;

  if (!strncmp (time, "smpte-24:", 9)) {
    framerate = 24.0;
    time += 9;
  } else if (!strncmp (time, "smpte-24-drop:", 14)) {
    framerate = 23.976;
    time += 14;
  } else if (!strncmp (time, "smpte-25:", 9)) {
    framerate = 25.0;
    time += 9;
  } else if (!strncmp (time, "smpte-30:", 9)) {
    framerate = 30.0;
    time += 9;
  } else if (!strncmp (time, "smpte-30-drop:", 14)) {
    framerate = 29.976;
    time += 14;
  } else if (!strncmp (time, "smpte-50:", 9)) {
    framerate = 50.0;
    time += 9;
  } else if (!strncmp (time, "smpte-60:", 9)) {
    framerate = 60.0;
    time += 9;
  } else if (!strncmp (time, "smpte-60-drop:", 14)) {
    framerate = 59.94;
    time += 14;
  } else {
    return GST_CLOCK_TIME_NONE;
  }

  fields = sscanf (time, "%d:%d:%d:%f", &hours, &minutes, &seconds, &frames);
  if (fields == 4) {
    if (hours < 0 || (guint) minutes > 59 || (guint) seconds > 59 ||
        frames < 0 || frames > ceil (framerate)) {
      res = GST_CLOCK_TIME_NONE;
    } else {
      hours_t = gst_util_uint64_scale (hours, GST_SECOND * 3600, 1);
      if (hours_t == G_MAXUINT64)
        goto overflow;

      res = ((minutes * 60) + seconds + (frames / framerate))
          * GST_SECOND;
      if (G_MAXUINT64 - hours_t < res)
        goto overflow;

      res = hours_t + res;
    }
  } else {
    res = GST_CLOCK_TIME_NONE;
  }

  return res;
overflow:
  return GST_CLOCK_TIME_NONE;
}

gchar *
gst_cmml_clock_time_to_npt (const GstClockTime time)
{
  guint seconds, hours, minutes, mseconds;
  gchar *res;

  g_return_val_if_fail (time != GST_CLOCK_TIME_NONE, NULL);

  hours = time / (GST_SECOND * 3600);
  minutes = (time / ((GST_SECOND * 60)) % 60);
  seconds = (time / GST_SECOND) % 60;
  mseconds = (time % GST_SECOND) / GST_MSECOND;

  if (mseconds < 100)
    mseconds *= 10;

  res = g_strdup_printf ("%u:%02u:%02u.%03u",
      hours, minutes, seconds, mseconds);

  return res;
}

gint64
gst_cmml_clock_time_to_granule (GstClockTime prev_time,
    GstClockTime current_time, gint64 granulerate_n, gint64 granulerate_d,
    guint8 granuleshift)
{
  gint64 keyindex, keyoffset, granulepos;
  gint64 granulerate;

  if (prev_time == GST_CLOCK_TIME_NONE)
    prev_time = 0;

  if (prev_time > current_time)
    return -1;

  granulerate = gst_util_uint64_scale (GST_SECOND,
      granulerate_n, granulerate_d);
  keyindex = prev_time / granulerate << granuleshift;
  keyoffset = (current_time - prev_time) / granulerate;
  granulepos = keyindex + keyoffset;

  return granulepos;
}

/* track list */
GHashTable *
gst_cmml_track_list_new ()
{
  return g_hash_table_new (g_str_hash, g_str_equal);
}

static gboolean
gst_cmml_track_list_destroy_track (gchar * key,
    GstCmmlTrack * track, gpointer user_data)
{
  GList *walk;

  for (walk = track->clips; walk; walk = g_list_next (walk))
    g_object_unref (G_OBJECT (walk->data));

  g_free (key);
  g_list_free (track->clips);
  g_free (track);

  return TRUE;
}

void
gst_cmml_track_list_destroy (GHashTable * tracks)
{
  g_hash_table_foreach_remove (tracks,
      (GHRFunc) gst_cmml_track_list_destroy_track, NULL);
  g_hash_table_destroy (tracks);
}

static gint
gst_cmml_track_list_compare_clips (GstCmmlTagClip * a, GstCmmlTagClip * b)
{
  if (a->start_time < b->start_time)
    return -1;

  return 1;
}

void
gst_cmml_track_list_add_clip (GHashTable * tracks, GstCmmlTagClip * clip)
{
  GstCmmlTrack *track = NULL;
  gchar *track_name = NULL;
  void *key = NULL, *value = NULL;

  /* find clip's track */
  g_hash_table_lookup_extended (tracks, clip->track, &key, &value);
  track_name = (gchar *) key;
  track = (GstCmmlTrack *) track;

  if (track_name == NULL)
    /* it doesn't exist yet: create its key */
    track_name = g_strdup ((gchar *) clip->track);

  if (track == NULL)
    track = g_new0 (GstCmmlTrack, 1);

  /* add clip to the tracklist */
  track->clips = g_list_insert_sorted (track->clips, g_object_ref (clip),
      (GCompareFunc) gst_cmml_track_list_compare_clips);

  /* reset the head every time as it could change */
  g_hash_table_insert (tracks, track_name, track);
}

gboolean
gst_cmml_track_list_del_clip (GHashTable * tracks, GstCmmlTagClip * clip)
{
  GstCmmlTrack *track;
  GList *link;
  gboolean res = FALSE;

  track = g_hash_table_lookup (tracks, clip->track);
  if (track) {
    link = g_list_find (track->clips, clip);
    if (link) {
      g_object_unref (G_OBJECT (link->data));
      track->clips = g_list_remove_link (track->clips, link);
      res = TRUE;
    }
  }

  return res;
}

gboolean
gst_cmml_track_list_has_clip (GHashTable * tracks, GstCmmlTagClip * clip)
{
  GstCmmlTrack *track;
  GList *walk;
  GstCmmlTagClip *tmp;
  gchar *clip_id = (gchar *) clip->id;
  gboolean res = FALSE;

  track = g_hash_table_lookup (tracks, clip_id);
  if (track) {
    for (walk = track->clips; walk; walk = g_list_next (walk)) {
      tmp = GST_CMML_TAG_CLIP (walk->data);
      if (!strcmp ((gchar *) tmp->id, clip_id)) {
        res = TRUE;
        break;
      }
    }
  }

  return res;
}

static gboolean
gst_cmml_track_list_merge_track (gchar * track_name,
    GstCmmlTrack * track, GList ** list)
{
  GList *walk;
  GstCmmlTagClip *cur;

  for (walk = track->clips; walk; walk = g_list_next (walk)) {
    cur = GST_CMML_TAG_CLIP (walk->data);
    *list = g_list_insert_sorted (*list, cur,
        (GCompareFunc) gst_cmml_track_list_compare_clips);
  }

  return TRUE;
}

GList *
gst_cmml_track_list_get_track_clips (GHashTable * tracks,
    const gchar * track_name)
{
  GstCmmlTrack *track;

  track = g_hash_table_lookup (tracks, track_name);
  return track ? track->clips : NULL;
}

GList *
gst_cmml_track_list_get_clips (GHashTable * tracks)
{
  GList *list = NULL;

  g_hash_table_foreach (tracks,
      (GHFunc) gst_cmml_track_list_merge_track, &list);
  return list;
}

GstCmmlTagClip *
gst_cmml_track_list_get_track_last_clip (GHashTable * tracks,
    const gchar * track_name)
{
  GstCmmlTrack *track;
  GList *res = NULL;

  track = g_hash_table_lookup (tracks, track_name);
  if (track && track->clips)
    res = g_list_last (track->clips);

  return res ? GST_CMML_TAG_CLIP (res->data) : NULL;
}

void
gst_cmml_track_list_set_data (GHashTable * tracks,
    const gchar * track_name, gpointer data)
{
  GstCmmlTrack *track;

  track = g_hash_table_lookup (tracks, track_name);
  if (track)
    track->user_data = data;
}

gpointer
gst_cmml_track_get_data (GHashTable * tracks, const gchar * track_name)
{
  GstCmmlTrack *track;

  track = g_hash_table_lookup (tracks, track_name);
  return track ? track->user_data : NULL;
}