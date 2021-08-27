GZDEC
=====

This is a example plugin for GStreamer-1.0 and GStreamer-0.10.

Bulid and test
--------------

### Manual (using host libraries)

* Go to the plugin's folder (`gzdec_latest` or `gzdec_0.10`)
* Check you have gstreamer-[0.10,1.0] libraries
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

* `make local_test`:        Build and test both versions
* `make local_test_latest`: Build and test the 1.x version
* `make local_test_0.10`:   Build and test the 0.10 version
