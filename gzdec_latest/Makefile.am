SUBDIRS = plugins

EXTRA_DIST = autogen.sh
ACLOCAL_AMFLAGS = -I m4

TEST_FILE = test_file

test:
	-@rm -f "$(TEST_FILE).out"
	GST_PLUGIN_PATH+=":$${PWD}/plugins/.libs/" \
		gst-launch-1.0 filesrc location=$(TEST_FILE).in.gz \
			! gzdec \
			! filesink location="$(TEST_FILE).out"
	diff -q "$(TEST_FILE).in" "$(TEST_FILE).out"

$(TEST_FILE).in:
	dd if=/dev/urandom of=$@ bs=1048576 count=16

$(TEST_FILE).in.gz: $(TEST_FILE).in
	gzip -c $^ > $@
