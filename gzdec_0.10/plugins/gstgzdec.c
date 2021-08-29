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
 * Free Software Foundation, Inc., 51 Franklin Street, Suite 500,
 * Boston, MA 02110-1335, USA.
 */
/**
 * SECTION:element-gstgzdec
 *
 * The gzdec element decompress gzip/bzip streams
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-0.10 filesrc location=file.txt.gz ! 'application/x-gzip' ! gzdec ! filesink location=file.txt
 * ]|
 * This pipeline decompress the file file.txt.gz into file.txt using zlib
 *
 * |[
 * gst-launch-0.10 filesrc location=file.txt.bz ! 'application/x-bzip' ! gzdec ! filesink location=file.txt
 * ]|
 * This pipeline decompress the file file.txt.bz into file.txt using bzlib2
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/gst.h>
#include "gstgzdec.h"

GST_DEBUG_CATEGORY_STATIC (gst_gzdec_debug_category);
#define GST_CAT_DEFAULT gst_gzdec_debug_category

static void gst_gzdec_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec);
static void gst_gzdec_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec);

static GstFlowReturn gst_gzdec_sink_chain (GstPad * pad, GstBuffer * buffer);


enum
{
  PROP_0
};

/* pad templates */

static GstStaticPadTemplate gst_gzdec_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/x-gzip; application/x-bzip")
    );

static GstStaticPadTemplate gst_gzdec_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/unknown")
    );


/* class initialization */

#define DEBUG_INIT(bla) \
  GST_DEBUG_CATEGORY_INIT (gst_gzdec_debug_category, "gzdec", 0, \
      "gzdec element");

GST_BOILERPLATE_FULL (GstGzdec, gst_gzdec, GstElement, GST_TYPE_ELEMENT,
    DEBUG_INIT);

static void
gst_gzdec_base_init (gpointer g_class)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (g_class);

  gst_element_class_add_static_pad_template (element_class,
      &gst_gzdec_sink_template);
  gst_element_class_add_static_pad_template (element_class,
      &gst_gzdec_src_template);

  gst_element_class_set_details_simple (element_class,
      "gzip decoder", "Generic", "gzip/bzip decoder",
      "Carlos Falgueras García <carlosfg@riseup.net");
}

static void
gst_gzdec_class_init (GstGzdecClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

  gobject_class->set_property = gst_gzdec_set_property;
  gobject_class->get_property = gst_gzdec_get_property;
}

static void
gst_gzdec_init (GstGzdec * gzdec, GstGzdecClass * gzdec_class)
{

  gzdec->sinkpad = gst_pad_new_from_static_template (&gst_gzdec_sink_template,
      "sink");
  gst_pad_set_chain_function (gzdec->sinkpad,
      GST_DEBUG_FUNCPTR (gst_gzdec_sink_chain));
  gst_element_add_pad (GST_ELEMENT (gzdec), gzdec->sinkpad);

  gzdec->srcpad = gst_pad_new_from_static_template (&gst_gzdec_src_template,
      "src");
  gst_element_add_pad (GST_ELEMENT (gzdec), gzdec->srcpad);
}

void
gst_gzdec_set_property (GObject * object, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
  GstGzdec *gzdec = GST_GZDEC (object);

  GST_DEBUG_OBJECT (gzdec, "set_property");

  switch (property_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_gzdec_get_property (GObject * object, guint property_id,
    GValue * value, GParamSpec * pspec)
{
  GstGzdec *gzdec = GST_GZDEC (object);

  GST_DEBUG_OBJECT (gzdec, "set_property");

  switch (property_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static GstFlowReturn
gst_gzdec_sink_chain (GstPad * pad, GstBuffer * buffer)
{
  GstFlowReturn ret;
  GstGzdec *gzdec;

  gzdec = GST_GZDEC (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (gzdec, "chain");

  ret = gst_pad_push (gzdec->srcpad, buffer);

  gst_object_unref (gzdec);
  return ret;
}


static gboolean
plugin_init (GstPlugin * plugin)
{
  return gst_element_register (plugin, "gzdec", GST_RANK_NONE, GST_TYPE_GZDEC);
}
