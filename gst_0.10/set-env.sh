#!/bin/bash -i
#
# this script is in git as gstreamer/scripts/gst-uninstalled
#
# It will set up the environment to use and develop gstreamer and projects
# that use gstreamer with an uninstalled git checkout of gstreamer and the
# plugin modules.
#
# It will set up LD_LIBRARY_PATH, DYLD_LIBRARY_PATH, PKG_CONFIG_PATH,
# GST_PLUGIN_PATH, GST_PLUGIN_SYSTEM_PATH, GST_REGISTRY, MANPATH, PYTHONPATH
# to prefer the uninstalled versions but also contain the installed ones.
# The only exception to this is, that no system installed plugins will be
# used but only the uninstalled ones.
#
# This script assumes that the relevant modules are checked out one by one
# under a given tree specified below in MYGST.
#
# Symlink this script in a directory in your path (for example $HOME/bin). You
# must name the symlink gst-something, where something is the subdirectory
# of MYGST that contains your gstreamer module checkouts.
#
# e.g.:
# - mkdir $HOME/gst/head
# - ln -sf gst-uninstalled $HOME/bin/gst-head
# - checkout copies of gstreamer modules in $HOME/gst/head
# - gst-head

# This script is run -i so that PS1 doesn't get cleared

# base path under which dirs are installed
GST="$(realpath "$(dirname "$0")/gstreamer")"
GST_PREFIX=$GST/prefix
if test ! -e $GST; then
  echo "$GST does not exist !"
  exit
fi

# set up a bunch of paths
PATH="\
$GST/gstreamer/tools:\
$GST/gst-plugins-base/tools:\
$GST/gst-player/src:\
$GST/gst-editor/src:\
$GST/gstreamer-sharp/tools:\
$GST_PREFIX/bin:\
$PATH"

# /some/path: makes the dynamic linker look in . too, so avoid this
LD_LIBRARY_PATH=$GST_PREFIX/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
DYLD_LIBRARY_PATH=$GST_PREFIX/lib${DYLD_LIBRARY_PATH:+:$DYLD_LIBRARY_PATH}
GI_TYPELIB_PATH=$GST_PREFIX/share/gir-1.0${GI_TYPELIB_PATH:+:$GI_TYPELIB_PATH}

# GStreamer rtsp server library
LD_LIBRARY_PATH=$GST/gst-rtsp-server/gst/rtsp-server/.libs:$LD_LIBRARY_PATH
DYLD_LIBRARY_PATH=$GST/gst-rtsp-server/gst/rtsp-server/.libs:$DYLD_LIBRARY_PATH
GI_TYPELIB_PATH=$GST/gst-rtsp-server/gst/rtsp-server:$GI_TYPELIB_PATH

# GStreamer ffmpeg libraries
for path in libavformat libavutil libavcodec libpostproc libavdevice
do
   LD_LIBRARY_PATH=$GST/gst-ffmpeg/gst-libs/ext/ffmpeg/$path:$LD_LIBRARY_PATH
   DYLD_LIBRARY_PATH=$GST/gst-ffmpeg/gst-libs/ext/ffmpeg/$path:$DYLD_LIBRARY_PATH
done

# GStreamer Editing Services library
LD_LIBRARY_PATH=$GST/gst-editing-services/ges/.libs:$LD_LIBRARY_PATH
DYLD_LIBRARY_PATH=$GST/gst-editing-services/ges/.libs:$DYLD_LIBRARY_PATH
PATH=$GST/gst-editing-services/tools:$PATH

# GStreamer plugins base libraries
for path in app audio cdda fft interfaces pbutils netbuffer riff rtp rtsp sdp tag utils video 
do
  LD_LIBRARY_PATH=$GST/gst-plugins-base/gst-libs/gst/$path/.libs:$LD_LIBRARY_PATH
  DYLD_LIBRARY_PATH=$GST/gst-plugins-base/gst-libs/gst/$path/.libs:$DYLD_LIBRARY_PATH
  GI_TYPELIB_PATH=$GST/gst-plugins-base/gst-libs/gst/$path:$GI_TYPELIB_PATH
done

# GStreamer plugins bad libraries
for path in basecamerabinsrc codecparsers interfaces
do
  LD_LIBRARY_PATH=$GST/gst-plugins-bad/gst-libs/gst/$path/.libs:$LD_LIBRARY_PATH
  DYLD_LIBRARY_PATH=$GST/gst-plugins-bad/gst-libs/gst/$path/.libs:$DYLD_LIBRARY_PATH
  GI_TYPELIB_PATH=$GST/gst-plugins-bad/gst-libs/gst/$path:$GI_TYPELIB_PATH
done

# GStreamer core libraries
for path in base net check controller dataprotocol
do
  LD_LIBRARY_PATH=$GST/gstreamer/libs/gst/$path/.libs:$LD_LIBRARY_PATH
  DYLD_LIBRARY_PATH=$GST/gstreamer/libs/gst/$path/.libs:$DYLD_LIBRARY_PATH
  GI_TYPELIB_PATH=$GST/gstreamer/libs/gst/$path:$GI_TYPELIB_PATH
done
LD_LIBRARY_PATH=$GST/gstreamer/gst/.libs:$LD_LIBRARY_PATH
DYLD_LIBRARY_PATH=$GST/gstreamer/gst/.libs:$DYLD_LIBRARY_PATH
GI_TYPELIB_PATH=$GST/gstreamer/gst:$GI_TYPELIB_PATH
export LD_LIBRARY_PATH
export DYLD_LIBRARY_PATH
export GI_TYPELIB_PATH
  
export PKG_CONFIG_PATH="\
$GST_PREFIX/lib/pkgconfig\
:$GST/gstreamer/pkgconfig\
:$GST/gst-plugins-base/pkgconfig\
:$GST/gst-plugins-good/pkgconfig\
:$GST/gst-plugins-ugly/pkgconfig\
:$GST/gst-plugins-bad/pkgconfig\
:$GST/gst-ffmpeg/pkgconfig\
:$GST/gst-python/pkgconfig\
:$GST/gst-rtsp-server/pkgconfig\
:$GST/gst-editing-services/pkgconfig\
:$GST/gstreamer-sharp/pkgconfig\
:$GST/farsight2\
:$GST/libnice/nice\
${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}"

export GST_PLUGIN_PATH="\
$GST/gstreamer/plugins\
:$GST/gst-plugins-base/ext\
:$GST/gst-plugins-base/gst\
:$GST/gst-plugins-base/sys\
:$GST/gst-plugins-good/ext\
:$GST/gst-plugins-good/gst\
:$GST/gst-plugins-good/sys\
:$GST/gst-plugins-ugly/ext\
:$GST/gst-plugins-ugly/gst\
:$GST/gst-plugins-ugly/sys\
:$GST/gst-plugins-bad/ext\
:$GST/gst-plugins-bad/gst\
:$GST/gst-plugins-bad/sys\
:$GST/gst-ffmpeg/ext\
:$GST/gnonlin/gnl\
:$GST/gst-omx/omx/.libs\
:$GST/gst-plugins-gl/gst\
:$GST/plugins\
:$GST/farsight2/gst\
:$GST/farsight2/transmitters\
:$GST/libnice/gst\
${GST_PLUGIN_PATH:+:$GST_PLUGIN_PATH}"

# don't use any system-installed plug-ins at all
export GST_PLUGIN_SYSTEM_PATH=
# set our registry somewhere else so we don't mess up the registry generated
# by an installed copy
rm -f $GST/gstreamer/registry.xml 2>/dev/null
export GST_REGISTRY=$GST/gstreamer/registry.dat
# Point at the uninstalled plugin scanner
export GST_PLUGIN_SCANNER=$GST/gstreamer/libs/gst/helpers/gst-plugin-scanner

# if we got a command, run it, else start a shell
if test ! -z "$1";
then
  $@
  exit $?
fi

# set up prompt to help us remember we're in a subshell, cd to
# the gstreamer base dir and start $SHELL
cd $GST
shell=$SHELL
if test "x$SHELL" = "x/bin/bash"
then
  # debian/ubuntu resets our PS1.  bastards.
  shell="$SHELL --noprofile"
fi
PS1="[gst] $PS1" $shell
