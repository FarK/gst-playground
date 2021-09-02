AC_PREREQ([2.53])

AC_INIT([gst-gzdec],[0.10.0])

dnl required versions of gstreamer and plugins-base
GST_REQUIRED=0.10.16
GSTPB_REQUIRED=0.10.16

AC_CONFIG_SRCDIR([plugins/gstgzdec.c])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([foreign 1.10])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AM_PROG_CC_C_O

LT_PREREQ([2.2.6])
LT_INIT

dnl give error and exit if we don't have pkgconfig
AC_CHECK_PROG(HAVE_PKGCONFIG, pkg-config, [ ], [
  AC_MSG_ERROR([You need to have pkg-config installed!])
])

dnl Check for the required version of GStreamer core (and gst-plugins-base)
PKG_CHECK_MODULES(GST, [
  gstreamer-0.10 >= $GST_REQUIRED
  gstreamer-base-0.10 >= $GST_REQUIRED
], [
  AC_SUBST(GST_CFLAGS)
  AC_SUBST(GST_LIBS)
], [
  AC_MSG_ERROR([
      The minimum version required for gstreamer core and gst-plugin base is
      $GST_REQUIRED.
  ])
])

PKG_CHECK_MODULES(XZLIB, [zlib, bzip2])

dnl set the plugindir where plugins should be installed (for plugins/Makefile.am)
if test "x${prefix}" = "x$HOME"; then
  plugindir="$HOME/.gstreamer-0.10/plugins"
else
  plugindir="\$(libdir)/gstreamer-0.10"
fi
AC_SUBST(plugindir)

dnl set proper LDFLAGS for plugins
GST_PLUGIN_LDFLAGS='-module -avoid-version -export-symbols-regex [_]*\(gst_\|Gst\|GST_\).*'
AC_SUBST(GST_PLUGIN_LDFLAGS)

AC_CONFIG_FILES([Makefile plugins/Makefile])
AC_OUTPUT
