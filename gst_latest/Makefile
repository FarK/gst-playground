all: build_gst

build_gst: gst-build/builddir/build.ninja
	ninja -C gst-build/builddir

gst-build/builddir/build.ninja: | gst-build/.git
	cd gst-build && \
	meson builddir -Dauto_features=disabled -Dgstreamer:tools=enabled

gst-build/.git:
	git clone --depth 1 \
		https://gitlab.freedesktop.org/gstreamer/gst-build.git

clean:
	ninja -C gst-build/builddir clean

mrproper:
	-rm -rf gst-build
