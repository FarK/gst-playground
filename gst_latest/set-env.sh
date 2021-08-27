#!/bin/bash

SCRIPT_DIR="$(realpath "$(dirname "$0")")"

"$SCRIPT_DIR/gst-build/gst-env.py" \
	--builddir "$SCRIPT_DIR/gst-build/builddir" \
	--srcdir "$(pwd)" \
	$@
