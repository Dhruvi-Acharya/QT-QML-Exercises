
#!/bin/bash
PKG_CONFIG_PATH="$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig" \
./configure --toolchain=msvc --target-os=win64 --arch=x86_64 --extra-cflags="-MD" --enable-shared --disable-static --disable-debug --disable-doc \
--enable-libdav1d \
--enable-libopenh264 \
--disable-dxva2 --enable-libmfx \
--enable-cuda --enable-cuvid --enable-ffnvcodec --enable-nvdec --enable-nvenc \
--extra-cflags="-I /usr/include -I /usr/local/include -I ../nv_sdk/v12.0/include" \
--extra-ldflags="-libpath:/usr/lib -libpath:/usr/local/lib -libpath:../nv_sdk/v12.0/lib/x64" \
--prefix=/ffmpeg_build_5.1_msvc_mfx_nvidia_dav1d_x64 --logfile=config.log



