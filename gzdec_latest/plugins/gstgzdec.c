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
 * gst-launch-1.0 filesrc location=file.txt.gz ! 'application/x-gzip' ! gzdec ! filesink location=file.txt
 * ]|
 * This pipeline decompress the file file.txt.gz into file.txt using zlib
 *
 * |[
 * gst-launch-1.0 filesrc location=file.txt.bz ! 'application/x-bzip' ! gzdec ! filesink location=file.txt
 * ]|
 * This pipeline decompress the file file.txt.bz into file.txt using bzlib2
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include "gstgzdec.h"

GST_DEBUG_CATEGORY_STATIC (gst_gzdec_debug);
#define GST_CAT_DEFAULT gst_gzdec_debug

/* prototypes */


static void gst_gzdec_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec);
static void gst_gzdec_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec);
static void gst_gzdec_state_changed (GstElement * element, GstState oldstate,
    GstState newstate, GstState pending);

static GstFlowReturn gst_gzdec_chain (GstPad * pad, GstObject * parent,
    GstBuffer * buffer);
static GstFlowReturn gst_gzdec_event (GstPad * pad, GstObject * parent,
    GstEvent * event);
enum
{
  PROP_0
};

/* pad templates */

static GstStaticPadTemplate src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/unknown")
    );

static GstStaticPadTemplate sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/x-gzip; application/x-bzip")
    );

#define gst_gzdec_parent_class parent_class
G_DEFINE_TYPE (GstGzdec, gst_gzdec, GST_TYPE_ELEMENT);


/* (b)zlib auxiliary methods */
#define XZ_ZLIB         0
#define XZ_BZLIB        1
#define XZ_ERROR        (1 << 0)
#define XZ_MORE_OUTPUT  (1 << 1)
#define XZ_CONTINUE     (1 << 2)
#define XZ_FINISH       (1 << 3)
#define XZ_MORE_INPUT   (1 << 4)

static void xzlib_init (GstGzdec * gzdec, int type);

static int zlib_init (GstGzdec * gzdec);
static void zlib_prepare_in_buffer (GstGzdec * gzdec, void *buf, size_t len);
static void zlib_prepare_out_buffer (GstGzdec * gzdec, void *buf, size_t len);
static size_t zlib_out_buffer_size (GstGzdec * gzdec);
static int zlib_uncompress_step (GstGzdec * gzdec);
static void zlib_free (GstGzdec * gzdec);

static int bzlib_init (GstGzdec * gzdec);
static void bzlib_prepare_in_buffer (GstGzdec * gzdec, void *buf, size_t len);
static void bzlib_prepare_out_buffer (GstGzdec * gzdec, void *buf, size_t len);
static size_t bzlib_out_buffer_size (GstGzdec * gzdec);
static int bzlib_uncompress_step (GstGzdec * gzdec);
static void bzlib_free (GstGzdec * gzdec);

static gboolean prepare_out_buffer (GstGzdec * gzdec, size_t in_buf_size);
static GstFlowReturn push_out_buf (GstGzdec * gzdec);

static void
gst_gzdec_class_init (GstGzdecClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstElementClass *gstelement_class = GST_ELEMENT_CLASS (klass);

  GST_DEBUG_CATEGORY_INIT (gst_gzdec_debug, "gzdec", 0, "gzdec element");

  gst_element_class_set_static_metadata (gstelement_class,
      "gzip decoder", "Generic", "gzip/bzip decoder",
      "Carlos Falgueras García <carlosfg@riseup.net");

  gst_element_class_add_static_pad_template (gstelement_class, &sink_template);
  gst_element_class_add_static_pad_template (gstelement_class, &src_template);

  gobject_class->set_property = gst_gzdec_set_property;
  gobject_class->get_property = gst_gzdec_get_property;
  gstelement_class->state_changed = gst_gzdec_state_changed;
}

static void
gst_gzdec_init (GstGzdec * gzdec)
{
  GstPadTemplate *tmpl;

  /* sinkpad */
  gzdec->sinkpad = gst_pad_new_from_static_template (&sink_template, "sink");
  gst_pad_set_chain_function (gzdec->sinkpad,
      GST_DEBUG_FUNCPTR (gst_gzdec_chain));
  gst_pad_set_event_function (gzdec->sinkpad,
      GST_DEBUG_FUNCPTR (gst_gzdec_event));
  gst_element_add_pad (GST_ELEMENT (gzdec), gzdec->sinkpad);

  /* srcpad */
  gzdec->srcpad = gst_pad_new_from_static_template (&src_template, "src");
  gst_element_add_pad (GST_ELEMENT (gzdec), gzdec->srcpad);

  gzdec->new_out_buf = TRUE;    // Force output buffer allocation at init
  gzdec->xz_initialized = FALSE;
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

  GST_DEBUG_OBJECT (gzdec, "get_property");

  switch (property_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gst_gzdec_state_changed (GstElement * element, GstState oldstate,
    GstState newstate, GstState pending)
{
  GstGzdec *gzdec = GST_GZDEC (element);

  if ((newstate == GST_STATE_NULL) && (gzdec->xz_initialized)) {
    gzdec->xz_free (gzdec);
  }
}

static gboolean
plugin_init (GstPlugin * plugin)
{
  return gst_element_register (plugin, "gzdec", GST_RANK_NONE, GST_TYPE_GZDEC);
}

static gboolean
prepare_out_buffer (GstGzdec * gzdec, size_t in_buf_size)
{
  if (!gzdec->new_out_buf)
    return TRUE;

  GST_DEBUG_OBJECT (gzdec, "Allocate new output buffer");
  gzdec->out_buf_capacity = in_buf_size;
  gzdec->out_buf =
      gst_buffer_new_allocate (NULL, gzdec->out_buf_capacity, NULL);

  if (!gzdec->out_buf)
    return FALSE;
  if (!gst_buffer_map (gzdec->out_buf, &gzdec->out_buf_map, GST_MAP_WRITE)) {
    gst_buffer_unref (gzdec->out_buf);
    return FALSE;
  }

  gzdec->new_out_buf = FALSE;

  gzdec->xz_prepare_out_buffer (gzdec,
      gzdec->out_buf_map.data, gzdec->out_buf_map.size);

  return TRUE;
}

static GstFlowReturn
push_out_buf (GstGzdec * gzdec)
{
  gst_buffer_unmap (gzdec->out_buf, &gzdec->out_buf_map);
  gst_buffer_set_size (gzdec->out_buf, gzdec->xz_out_buffer_size (gzdec));
  gzdec->new_out_buf = TRUE;
  return gst_pad_push (gzdec->srcpad, gzdec->out_buf);
}

static GstFlowReturn
gst_gzdec_chain (GstPad * pad, GstObject * parent, GstBuffer * in_buf)
{
  GstGzdec *gzdec = GST_GZDEC (parent);
  GstMapInfo in_buf_map;
  GstFlowReturn ret = GST_FLOW_ERROR;
  GstEvent *eos;
  int xz_ret;

  GST_DEBUG_OBJECT (gzdec, "New input buffer");
  if (!gst_buffer_map (in_buf, &in_buf_map, GST_MAP_READ))
    goto free_in;

  gzdec->xz_prepare_in_buffer (gzdec, in_buf_map.data, in_buf_map.size);

  // Keep decompressing and pushing buffers until finish, error or input exhaust
  do {
    // Allocate new output buffer if necessary
    if (!prepare_out_buffer (gzdec, in_buf_map.size))
      goto unmap_in;

    // Uncompress until error, input exhaust, output full or finish
    GST_DEBUG_OBJECT (gzdec, "Uncompress step");
    xz_ret = gzdec->xz_uncompress_step (gzdec);

    if (xz_ret & XZ_ERROR)
      goto free_out;

    // Output buffer is full, push it and continue
    if (xz_ret & XZ_MORE_OUTPUT) {
      GST_DEBUG_OBJECT (gzdec, "Out buffer ready. Push it");
      ret = push_out_buf (gzdec);
      if (ret < 0)
        goto unmap_in;
    }

    if (xz_ret & XZ_FINISH) {
      GST_DEBUG_OBJECT (gzdec, "Decompression finish. Send EOS");
      eos = gst_event_new_eos ();
      gst_pad_push_event (gzdec->srcpad, eos);
      ret = GST_FLOW_EOS;
      goto unmap_in;
    }
  } while (!(xz_ret & XZ_MORE_INPUT));

  ret = GST_FLOW_OK;
  goto unmap_in;

free_out:
  gst_buffer_unmap (gzdec->out_buf, &gzdec->out_buf_map);
  gst_buffer_unref (gzdec->out_buf);
unmap_in:
  gst_buffer_unmap (in_buf, &in_buf_map);
free_in:
  gst_buffer_unref (in_buf);
  return ret;
}

static GstFlowReturn
gst_gzdec_event (GstPad * pad, GstObject * parent, GstEvent * event)
{
  GstGzdec *gzdec = GST_GZDEC (parent);
  const gchar *mtype;
  GstStructure *structure;
  GstCaps *caps;
  int lib;

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_CAPS:
      gst_event_parse_caps (event, &caps);
      GST_DEBUG_OBJECT (gzdec, "setcaps %" GST_PTR_FORMAT, caps);
      structure = gst_caps_get_structure (caps, 0);
      mtype = gst_structure_get_name (structure);

      if (g_str_equal (mtype, "application/x-gzip")) {
        GST_DEBUG_OBJECT (gzdec, "GZIP stream");
        lib = XZ_ZLIB;
      } else if (g_str_equal (mtype, "application/x-bzip")) {
        GST_DEBUG_OBJECT (gzdec, "BZIP stream");
        lib = XZ_BZLIB;
      } else {
        GST_DEBUG_OBJECT (gzdec, "Invalid caps");
        gst_event_unref (event);
        return FALSE;
      }
      xzlib_init (gzdec, lib);
      break;
  };

  return gst_pad_event_default (pad, parent, event);
}

static void
xzlib_init (GstGzdec * gzdec, int type)
{
  if (type == XZ_BZLIB) {
    gzdec->xz_prepare_in_buffer  = bzlib_prepare_in_buffer;
    gzdec->xz_prepare_out_buffer = bzlib_prepare_out_buffer;
    gzdec->xz_uncompress_step    = bzlib_uncompress_step;
    gzdec->xz_out_buffer_size    = bzlib_out_buffer_size;
    gzdec->xz_free               = bzlib_free;

    bzlib_init (gzdec);
  } else {
    gzdec->xz_prepare_in_buffer  = zlib_prepare_in_buffer;
    gzdec->xz_prepare_out_buffer = zlib_prepare_out_buffer;
    gzdec->xz_uncompress_step    = zlib_uncompress_step;
    gzdec->xz_out_buffer_size    = zlib_out_buffer_size;
    gzdec->xz_free               = zlib_free;

    zlib_init (gzdec);
  }
  gzdec->xz_initialized = TRUE;
}

static int
zlib_init (GstGzdec * gzdec)
{
  GST_DEBUG_OBJECT (gzdec, "zlib init");
  gzdec->zstrm.next_in   = Z_NULL;
  gzdec->zstrm.avail_in  = 0;
  gzdec->zstrm.next_out  = Z_NULL;
  gzdec->zstrm.avail_out = 0;
  gzdec->zstrm.zalloc    = NULL;
  gzdec->zstrm.zfree     = NULL;

  return inflateInit2 (&gzdec->zstrm, MAX_WBITS + 16);
}

static void
zlib_prepare_in_buffer (GstGzdec * gzdec, void *buf, size_t len)
{
  gzdec->zstrm.next_in  = buf;
  gzdec->zstrm.avail_in = len;
}

static void
zlib_prepare_out_buffer (GstGzdec * gzdec, void *buf, size_t len)
{
  gzdec->zstrm.next_out  = buf;
  gzdec->zstrm.avail_out = len;
}

static size_t
zlib_out_buffer_size (GstGzdec * gzdec)
{
  return gzdec->out_buf_capacity - gzdec->zstrm.avail_out;
}

static int
zlib_uncompress_step (GstGzdec * gzdec)
{
  int ret;
  int err;

  ret = 0;

  err = inflate (&gzdec->zstrm, Z_SYNC_FLUSH);
  if ((err < 0) && (err != Z_BUF_ERROR)) {
    GST_DEBUG_OBJECT (gzdec, "Uncompress error: \"%s\"\n", zError (err));
    if (gzdec->zstrm.msg)
      GST_DEBUG_OBJECT (gzdec, "%s\n", gzdec->zstrm.msg);
    return XZ_ERROR;
  }

  if (err == Z_STREAM_END) {
    ret |= XZ_FINISH;
    if (gzdec->zstrm.avail_out > 0)
      ret |= XZ_MORE_OUTPUT;
  }
  if (gzdec->zstrm.avail_out == 0)
    ret |= XZ_MORE_OUTPUT;
  if (gzdec->zstrm.avail_in == 0)
    ret |= XZ_MORE_INPUT;

  return ret;
}

static void
zlib_free (GstGzdec * gzdec)
{
  GST_DEBUG_OBJECT (gzdec, "zlib free");
  inflateEnd (&gzdec->zstrm);
}

static int
bzlib_init (GstGzdec * gzdec)
{
  GST_DEBUG_OBJECT (gzdec, "bzlib init");
  gzdec->bzstrm.next_in   = NULL;
  gzdec->bzstrm.avail_in  = 0;
  gzdec->bzstrm.next_out  = NULL;
  gzdec->bzstrm.avail_out = 0;
  gzdec->bzstrm.bzalloc   = NULL;
  gzdec->bzstrm.bzfree    = NULL;

  return BZ2_bzDecompressInit (&gzdec->bzstrm, 0, 0);
}

static void
bzlib_prepare_in_buffer (GstGzdec * gzdec, void *buf, size_t len)
{
  gzdec->bzstrm.next_in  = buf;
  gzdec->bzstrm.avail_in = len;
}

static void
bzlib_prepare_out_buffer (GstGzdec * gzdec, void *buf, size_t len)
{
  gzdec->bzstrm.next_out  = buf;
  gzdec->bzstrm.avail_out = len;
}

static size_t
bzlib_out_buffer_size (GstGzdec * gzdec)
{
  return gzdec->out_buf_capacity - gzdec->bzstrm.avail_out;
}

static int
bzlib_uncompress_step (GstGzdec * gzdec)
{
  int ret = 0;
  int err;

  err = BZ2_bzDecompress (&gzdec->bzstrm);
  if ((err != BZ_OK) && (err != BZ_STREAM_END)) {
    GST_DEBUG_OBJECT (gzdec, "Uncompress error: \"%d\"\n", err);
    return XZ_ERROR;
  }

  if (err == BZ_STREAM_END) {
    ret |= XZ_FINISH;
    if (gzdec->bzstrm.avail_out > 0)
      ret |= XZ_MORE_OUTPUT;
  }
  if (gzdec->bzstrm.avail_out == 0)
    ret |= XZ_MORE_OUTPUT;
  if (gzdec->bzstrm.avail_in == 0)
    ret |= XZ_MORE_INPUT;

  return ret;
}

static void
bzlib_free (GstGzdec * gzdec)
{
  GST_DEBUG_OBJECT (gzdec, "bzlib free");
  BZ2_bzDecompressEnd (&gzdec->bzstrm);
}
