GZDEC
=====

This is a example plugin for GStreamer-1.0 and GStreamer-0.10.

Bulid and test
--------------

The top makefile is just a wrapper for automate the build and test of both
versions of the plugin (1.x and 0.10), but each plugin has a independent project
by itself and has it own documentation. You can check it in `gzdec_*/README`.

### Manual (using host libraries)

* Go to the plugin's folder (`gzdec_latest` or `gzdec_0.10`)
* Check you have gstreamer-[0.10,1.0], zlib and bzip2 libraries
* `./autogen.sh`
* `make`
* And finally, after `make test` you should see the result of the lauch of a
  simple pipeline like this:
  `filesrc location=file.txt.gz ! gzdec ! filesink location="file.txt"`

### Automatic with host GStreamer

You can use the following make target to automatically do the same as before:

* `make test`:        Build and test both versions
* `make test_latest`: Build and test the 1.x version
* `make test_0.10`:   Build and test the 0.10 version

### Automatic with downloaded GStreamer

You can use the following make target in order to automatically download the
appropriate GStreamer version, build the plugin and launch the test:

* `make env_test`:        Build and test both versions
* `make env_test_latest`: Build and test the 1.x version
* `make env_test_0.10`:   Build and test the 0.10 version

### Dependencies

If you try the local build, the only dependencies you need are:
  * gstreamer-1.0: For the 1.0 version
  * gstreamer-0.10: For the 0.10 version
  * autotools
  * zlib
  * bzip2

If you want to use a downloaded gstreamer, you need the dependencies to build
it. Some of them are:
  * meson (only for 1.0)
  * ninja (only for 1.0)
  * flex
  * bison
  * gtk-doc
  * libxml2
