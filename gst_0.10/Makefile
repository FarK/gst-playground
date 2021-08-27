all: build_gst

build_gst: gstreamer/prefix
gstreamer/prefix: gstreamer/Makefile
	$(MAKE) -C gstreamer install

gstreamer/Makefile: | gstreamer/.git
	cd gstreamer && \
	./autogen.sh && \
	./configure --prefix="$${PWD}/prefix"

gstreamer/.git:
	git clone --branch=0.10 --depth=1 \
		https://gitlab.freedesktop.org/gstreamer/gstreamer.git

clean:
	-$(MAKE) -C gstreamer clean

mrproper:
	-rm -rf gstreamer
