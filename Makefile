all: test


# Build and test using host libraries
test: test_latest test_0.10
test_%: build_%
	$(MAKE) -C "gzdec_$*" test

build_%: gzdec_%/configure
	$(MAKE) -C "gzdec_$*"

gzdec_%/configure: autogen_%
	cd "gzdec_$*" && ./configure

autogen_%:
	cd "gzdec_$*" && ./autogen.sh


# Build and test using downloaded gstreamer and changed environment
env_test: env_test_latest env_test_0.10
env_test_%: deps_%
	./gst_$*/set-env.sh make test_$*

deps: deps_latest deps_0.10
deps_%:
	$(MAKE) -C "gst_$*"


# Cleaning targets
clean: clean_latest clean_0.10
clean_%:
	$(MAKE) -C "gzdec_$*" clean

mrproper: mrproper_latest mrproper_0.10
mrproper_%: clean_%
	$(MAKE) -C "gst_$*" mrproper
	cd "gzdec_$*" && git clean -fdx
