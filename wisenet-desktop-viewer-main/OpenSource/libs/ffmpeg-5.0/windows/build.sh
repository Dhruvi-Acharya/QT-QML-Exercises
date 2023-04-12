
#!/bin/bash
./configure --toolchain=msvc --target-os=win64 --arch=x86_64 --extra-cflags="-MD" --enable-shared --disable-static --disable-debug --disable-doc \
--disable-dxva2 \
--enable-libmfx \
--enable-libdav1d \
--enable-cuda --enable-cuvid --enable-ffnvcodec --enable-nvdec --enable-nvenc \
--extra-cflags="-I /usr/include -I /usr/local/include -I ../nv_sdk/v11.2/include"  \
--extra-ldflags="-libpath:/usr/lib -libpath:/usr/local/lib -libpath:../nv_sdk/v11.2/lib/x64 -libpath:../msdk/build/win_x64" \
--prefix=/ffmpeg_build_5.0_msvc_mfx_nvidia_dav1d_x64 --logfile=config.log



