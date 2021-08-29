/* GStreamer
 * Copyright (C) 2021 Carlos Falgueras García <carlosfg@riseup.net>
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

#ifndef _GST_GZDEC_H_
#define _GST_GZDEC_H_

#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_TYPE_GZDEC          (gst_gzdec_get_type ())
#define GST_GZDEC(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_GZDEC, GstGzdec))
#define GST_GZDEC_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_GZDEC, GstGzdecClass))
#define GST_IS_GZDEC(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_GZDEC))
#define GST_IS_GZDEC_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_GZDEC))

typedef struct _GstGzdec GstGzdec;
typedef struct _GstGzdecClass GstGzdecClass;

struct _GstGzdec
{
  GstElement base_gzdec;

  GstPad *sinkpad;
  GstPad *srcpad;
};

struct _GstGzdecClass
{
  GstElementClass base_gzdec_class;
};

GType gst_gzdec_get_type (void);

G_END_DECLS

#endif
