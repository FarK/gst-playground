SUBDIRS = plugins

EXTRA_DIST = autogen.sh
ACLOCAL_AMFLAGS = -I m4

TEST_FILE = test_file

test: test-gz test-bz

test-%z: $(TEST_FILE).in.%z
	-@rm -f "$(TEST_FILE).$*z-out"
	GST_PLUGIN_PATH+=":$${PWD}/plugins/.libs/" \
		gst-launch-0.10 -ve filesrc location=$(TEST_FILE).in.$*z \
			! 'application/x-$*zip' \
			! gzdec \
			! filesink location="$(TEST_FILE).$*z-out"
	diff -q "$(TEST_FILE).in" "$(TEST_FILE).$*z-out"

$(TEST_FILE).in:
	dd if=/dev/urandom of=$@ bs=1048576 count=16

$(TEST_FILE).in.gz: $(TEST_FILE).in
	gzip -c $^ > $@

$(TEST_FILE).in.bz: $(TEST_FILE).in
	bzip2 -c $^ > $@
