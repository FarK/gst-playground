plugin_LTLIBRARIES = libgstgzdec.la

# sources used to compile this plug-in
libgstgzdec_la_SOURCES = gstgzdecplugin.c gstgzdec.c gstgzdec.h

# compiler and linker flags used to compile this plugin, set in configure.ac
libgstgzdec_la_CFLAGS = $(GST_CFLAGS) $(XZLIB_LIBS) -Wno-deprecated-declarations
libgstgzdec_la_LIBADD = $(GST_LIBS)
libgstgzdec_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstgzdec_la_LIBTOOLFLAGS = --tag=disable-static
