#!/bin/sh

SOURCE_PATH="$(pwd)"
RELEASE_DIR="ffmpeg_release"
FFMPEG_LIB_PATH="$SOURCE_PATH/$RELEASE_DIR/lib"
FFMPEG_cLIB_PATH="$SOURCE_PATH/$RELEASE_DIR/lib_copy"

#./configure --prefix=ffmpeg_release --enable-shared --disable-autodetect
./configure --prefix=ffmpeg_release --enable-shared --disable-autodetect --enable-videotoolbox --enable-libdav1d --logfile=config.log
make
make install

rm -rf $FFMPEG_cLIB_PATH
mkdir -p $FFMPEG_cLIB_PATH
cp $FFMPEG_LIB_PATH/libavcodec.59.*.dylib $FFMPEG_cLIB_PATH/libavcodec.dylib
cp $FFMPEG_LIB_PATH/libavdevice.59.*.dylib $FFMPEG_cLIB_PATH/libavdevice.dylib
cp $FFMPEG_LIB_PATH/libavfilter.8.*.dylib $FFMPEG_cLIB_PATH/libavfilter.dylib
cp $FFMPEG_LIB_PATH/libavformat.59.*.dylib $FFMPEG_cLIB_PATH/libavformat.dylib
cp $FFMPEG_LIB_PATH/libavutil.57.*.dylib $FFMPEG_cLIB_PATH/libavutil.dylib
cp $FFMPEG_LIB_PATH/libswresample.4.*.dylib $FFMPEG_cLIB_PATH/libswresample.dylib
cp $FFMPEG_LIB_PATH/libswscale.6.*.dylib $FFMPEG_cLIB_PATH/libswscale.dylib

cd $FFMPEG_cLIB_PATH

install_name_tool -id @rpath/libavcodec.dylib libavcodec.dylib
install_name_tool -id @rpath/libavdevice.dylib libavdevice.dylib
install_name_tool -id @rpath/libavfilter.dylib libavfilter.dylib
install_name_tool -id @rpath/libavformat.dylib libavformat.dylib
install_name_tool -id @rpath/libavutil.dylib libavutil.dylib
install_name_tool -id @rpath/libswresample.dylib libswresample.dylib
install_name_tool -id @rpath/libswscale.dylib libswscale.dylib

install_name_tool -change $RELEASE_DIR/lib/libswresample.4.dylib @rpath/libswresample.dylib libavcodec.dylib
install_name_tool -change $RELEASE_DIR/lib/libavutil.57.dylib @rpath/libavutil.dylib libavcodec.dylib

install_name_tool -change $RELEASE_DIR/lib/libavfilter.8.dylib @rpath/libavfilter.dylib libavdevice.dylib
install_name_tool -change $RELEASE_DIR/lib/libswscale.6.dylib @rpath/libswscale.dylib libavdevice.dylib
install_name_tool -change $RELEASE_DIR/lib/libavformat.59.dylib @rpath/libavformat.dylib libavdevice.dylib
install_name_tool -change $RELEASE_DIR/lib/libavcodec.59.dylib @rpath/libavcodec.dylib libavdevice.dylib
install_name_tool -change $RELEASE_DIR/lib/libswresample.4.dylib @rpath/libswresample.dylib libavdevice.dylib
install_name_tool -change $RELEASE_DIR/lib/libavutil.57.dylib @rpath/libavutil.dylib libavdevice.dylib

install_name_tool -change $RELEASE_DIR/lib/libswscale.6.dylib @rpath/libswscale.dylib libavfilter.dylib
install_name_tool -change $RELEASE_DIR/lib/libavformat.59.dylib @rpath/libavformat.dylib libavfilter.dylib
install_name_tool -change $RELEASE_DIR/lib/libavcodec.59.dylib @rpath/libavcodec.dylib libavfilter.dylib
install_name_tool -change $RELEASE_DIR/lib/libswresample.4.dylib @rpath/libswresample.dylib libavfilter.dylib
install_name_tool -change $RELEASE_DIR/lib/libavutil.57.dylib @rpath/libavutil.dylib libavfilter.dylib

install_name_tool -change $RELEASE_DIR/lib/libavcodec.59.dylib @rpath/libavcodec.dylib libavformat.dylib
install_name_tool -change $RELEASE_DIR/lib/libswresample.4.dylib @rpath/libswresample.dylib libavformat.dylib
install_name_tool -change $RELEASE_DIR/lib/libavutil.57.dylib @rpath/libavutil.dylib libavformat.dylib

install_name_tool -change $RELEASE_DIR/lib/libavutil.57.dylib @rpath/libavutil.dylib libswresample.dylib
install_name_tool -change $RELEASE_DIR/lib/libavutil.57.dylib @rpath/libavutil.dylib libswscale.dylib

cd $SOURCE_PATH
