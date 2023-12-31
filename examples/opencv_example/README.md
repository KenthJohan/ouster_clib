# OpenCV example

This example receive frames from sensor and converts it to `cv::Mat` and then draws it on screen.


## Requirement
```bash
sudo apt update
sudo apt install git build-essential libcurl4-openssl-dev  libglfw3-dev libglew-dev libopencv-dev
```


## ldd
```bash
$ ldd examples/opencv_example/bin/x64-Linux-debug/opencv_example 
        linux-vdso.so.1 (0x00007ffee71f3000)
        libopencv_core.so.406 => /lib/x86_64-linux-gnu/libopencv_core.so.406 (0x00007f09a5c00000)
        libopencv_highgui.so.406 => /lib/x86_64-linux-gnu/libopencv_highgui.so.406 (0x00007f09a6019000)
        libGL.so.1 => /lib/x86_64-linux-gnu/libGL.so.1 (0x00007f09a5f92000)
        libstdc++.so.6 => /lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007f09a5800000)
        libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007f09a5f6e000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f09a5400000)
        libz.so.1 => /lib/x86_64-linux-gnu/libz.so.1 (0x00007f09a5be2000)
        libGLX.so.0 => /lib/x86_64-linux-gnu/libGLX.so.0 (0x00007f09a5bb0000)
        liblapack.so.3 => /lib/x86_64-linux-gnu/liblapack.so.3 (0x00007f09a4c00000)
        libblas.so.3 => /lib/x86_64-linux-gnu/libblas.so.3 (0x00007f09a5b05000)
        libtbb.so.12 => /lib/x86_64-linux-gnu/libtbb.so.12 (0x00007f09a5ab5000)
        libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007f09a5717000)
        libOpenGL.so.0 => /lib/x86_64-linux-gnu/libOpenGL.so.0 (0x00007f09a5a8a000)
        libQt5Test.so.5 => /lib/x86_64-linux-gnu/libQt5Test.so.5 (0x00007f09a56bf000)
        libQt5OpenGL.so.5 => /lib/x86_64-linux-gnu/libQt5OpenGL.so.5 (0x00007f09a5662000)
        libopencv_imgcodecs.so.406 => /lib/x86_64-linux-gnu/libopencv_imgcodecs.so.406 (0x00007f09a5608000)
        libopencv_imgproc.so.406 => /lib/x86_64-linux-gnu/libopencv_imgproc.so.406 (0x00007f09a4600000)
        libQt5Widgets.so.5 => /lib/x86_64-linux-gnu/libQt5Widgets.so.5 (0x00007f09a3e00000)
        libQt5Gui.so.5 => /lib/x86_64-linux-gnu/libQt5Gui.so.5 (0x00007f09a3600000)
        libQt5Core.so.5 => /lib/x86_64-linux-gnu/libQt5Core.so.5 (0x00007f09a3000000)
        libGLdispatch.so.0 => /lib/x86_64-linux-gnu/libGLdispatch.so.0 (0x00007f09a4b48000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f09a609a000)
        libX11.so.6 => /lib/x86_64-linux-gnu/libX11.so.6 (0x00007f09a44c2000)
        libgfortran.so.5 => /lib/x86_64-linux-gnu/libgfortran.so.5 (0x00007f09a2c00000)
        libjpeg.so.8 => /lib/x86_64-linux-gnu/libjpeg.so.8 (0x00007f09a3d7d000)
        libwebp.so.7 => /lib/x86_64-linux-gnu/libwebp.so.7 (0x00007f09a5391000)
        libpng16.so.16 => /lib/x86_64-linux-gnu/libpng16.so.16 (0x00007f09a3d45000)
        libgdcmMSFF.so.3.0 => /lib/x86_64-linux-gnu/libgdcmMSFF.so.3.0 (0x00007f09a2800000)
        libtiff.so.6 => /lib/x86_64-linux-gnu/libtiff.so.6 (0x00007f09a3572000)
        libopenjp2.so.7 => /lib/x86_64-linux-gnu/libopenjp2.so.7 (0x00007f09a3ce4000)
        libOpenEXR-3_1.so.30 => /lib/x86_64-linux-gnu/libOpenEXR-3_1.so.30 (0x00007f09a2400000)
        libgdal.so.32 => /lib/x86_64-linux-gnu/libgdal.so.32 (0x00007f09a0e00000)
        libgdcmDSED.so.3.0 => /lib/x86_64-linux-gnu/libgdcmDSED.so.3.0 (0x00007f09a2f0a000)
        libharfbuzz.so.0 => /lib/x86_64-linux-gnu/libharfbuzz.so.0 (0x00007f09a2b18000)
        libmd4c.so.0 => /lib/x86_64-linux-gnu/libmd4c.so.0 (0x00007f09a5a78000)
        libdouble-conversion.so.3 => /lib/x86_64-linux-gnu/libdouble-conversion.so.3 (0x00007f09a2ef6000)
        libicui18n.so.72 => /lib/x86_64-linux-gnu/libicui18n.so.72 (0x00007f09a0a00000)
        libicuuc.so.72 => /lib/x86_64-linux-gnu/libicuuc.so.72 (0x00007f09a0801000)
        libpcre2-16.so.0 => /lib/x86_64-linux-gnu/libpcre2-16.so.0 (0x00007f09a2a8a000)
        libzstd.so.1 => /lib/x86_64-linux-gnu/libzstd.so.1 (0x00007f09a274c000)
        libglib-2.0.so.0 => /lib/x86_64-linux-gnu/libglib-2.0.so.0 (0x00007f09a06ba000)
        libxcb.so.1 => /lib/x86_64-linux-gnu/libxcb.so.1 (0x00007f09a2a60000)
        libgdcmDICT.so.3.0 => /lib/x86_64-linux-gnu/libgdcmDICT.so.3.0 (0x00007f09a0200000)
        libgdcmjpeg8.so.3.0 => /lib/x86_64-linux-gnu/libgdcmjpeg8.so.3.0 (0x00007f09a2a35000)
        libgdcmjpeg12.so.3.0 => /lib/x86_64-linux-gnu/libgdcmjpeg12.so.3.0 (0x00007f09a2721000)
        libgdcmjpeg16.so.3.0 => /lib/x86_64-linux-gnu/libgdcmjpeg16.so.3.0 (0x00007f09a23d5000)
        libcharls.so.2 => /lib/x86_64-linux-gnu/libcharls.so.2 (0x00007f09a0daf000)
        libuuid.so.1 => /lib/x86_64-linux-gnu/libuuid.so.1 (0x00007f09a5a6f000)
        libjson-c.so.5 => /lib/x86_64-linux-gnu/libjson-c.so.5 (0x00007f09a270e000)
        libgdcmIOD.so.3.0 => /lib/x86_64-linux-gnu/libgdcmIOD.so.3.0 (0x00007f09a23bf000)
        libgdcmCommon.so.3.0 => /lib/x86_64-linux-gnu/libgdcmCommon.so.3.0 (0x00007f09a0d8a000)
        liblzma.so.5 => /lib/x86_64-linux-gnu/liblzma.so.5 (0x00007f09a0d58000)
        libLerc.so.4 => /lib/x86_64-linux-gnu/libLerc.so.4 (0x00007f09a0635000)
        libjbig.so.0 => /lib/x86_64-linux-gnu/libjbig.so.0 (0x00007f09a5380000)
        libdeflate.so.0 => /lib/x86_64-linux-gnu/libdeflate.so.0 (0x00007f09a23ac000)
        libImath-3_1.so.29 => /lib/x86_64-linux-gnu/libImath-3_1.so.29 (0x00007f09a05e5000)
        libIlmThread-3_1.so.30 => /lib/x86_64-linux-gnu/libIlmThread-3_1.so.30 (0x00007f09a3569000)
        libIex-3_1.so.30 => /lib/x86_64-linux-gnu/libIex-3_1.so.30 (0x00007f09a0569000)
        libcurl.so.4 => /lib/x86_64-linux-gnu/libcurl.so.4 (0x00007f09a0154000)
        libodbc.so.2 => /lib/x86_64-linux-gnu/libodbc.so.2 (0x00007f09a04f8000)
        libodbcinst.so.2 => /lib/x86_64-linux-gnu/libodbcinst.so.2 (0x00007f09a0d43000)
        libxml2.so.2 => /lib/x86_64-linux-gnu/libxml2.so.2 (0x00007f099ff6a000)
        libcrypto.so.3 => /lib/x86_64-linux-gnu/libcrypto.so.3 (0x00007f099fa00000)
        liblz4.so.1 => /lib/x86_64-linux-gnu/liblz4.so.1 (0x00007f099ff47000)
        libblosc.so.1 => /lib/x86_64-linux-gnu/libblosc.so.1 (0x00007f09a2ee8000)
        libarmadillo.so.11 => /lib/libarmadillo.so.11 (0x00007f09a04e5000)
        libqhull_r.so.8.0 => /lib/x86_64-linux-gnu/libqhull_r.so.8.0 (0x00007f099fed4000)
        libxerces-c-3.2.so => /lib/x86_64-linux-gnu/libxerces-c-3.2.so (0x00007f099f600000)
        libgeotiff.so.5 => /lib/x86_64-linux-gnu/libgeotiff.so.5 (0x00007f099fe9f000)
        libheif.so.1 => /lib/x86_64-linux-gnu/libheif.so.1 (0x00007f099f974000)
        libpoppler.so.126 => /lib/x86_64-linux-gnu/libpoppler.so.126 (0x00007f099f200000)
        libgif.so.7 => /lib/x86_64-linux-gnu/libgif.so.7 (0x00007f09a2a2a000)
        libnetcdf.so.19 => /lib/x86_64-linux-gnu/libnetcdf.so.19 (0x00007f099f01e000)
        libcfitsio.so.10 => /lib/x86_64-linux-gnu/libcfitsio.so.10 (0x00007f099ec00000)
        libhdf5_serial.so.103 => /lib/x86_64-linux-gnu/libhdf5_serial.so.103 (0x00007f099e800000)
        libsqlite3.so.0 => /lib/x86_64-linux-gnu/libsqlite3.so.0 (0x00007f099e6ae000)
        libpq.so.5 => /lib/x86_64-linux-gnu/libpq.so.5 (0x00007f099f5ad000)
        libkmlbase.so.1 => /lib/x86_64-linux-gnu/libkmlbase.so.1 (0x00007f099fe84000)
        libkmldom.so.1 => /lib/x86_64-linux-gnu/libkmldom.so.1 (0x00007f099ef90000)
        libkmlengine.so.1 => /lib/x86_64-linux-gnu/libkmlengine.so.1 (0x00007f099fe55000)
        libfyba.so.0 => /lib/x86_64-linux-gnu/libfyba.so.0 (0x00007f099ef3a000)
        libpcre2-8.so.0 => /lib/x86_64-linux-gnu/libpcre2-8.so.0 (0x00007f099e614000)
        libspatialite.so.7 => /lib/x86_64-linux-gnu/libspatialite.so.7 (0x00007f099de00000)
        libmysqlclient.so.21 => /lib/x86_64-linux-gnu/libmysqlclient.so.21 (0x00007f099d600000)
        libfreexl.so.1 => /lib/x86_64-linux-gnu/libfreexl.so.1 (0x00007f09a2702000)
        libogdi.so.4.1 => /lib/libogdi.so.4.1 (0x00007f099f958000)
        libexpat.so.1 => /lib/x86_64-linux-gnu/libexpat.so.1 (0x00007f099f582000)
        libgeos_c.so.1 => /lib/x86_64-linux-gnu/libgeos_c.so.1 (0x00007f099ebc2000)
        libproj.so.25 => /lib/x86_64-linux-gnu/libproj.so.25 (0x00007f099d200000)
        libdfalt.so.0 => /lib/libdfalt.so.0 (0x00007f099dd59000)
        libmfhdfalt.so.0 => /lib/libmfhdfalt.so.0 (0x00007f099ef10000)
        libfreetype.so.6 => /lib/x86_64-linux-gnu/libfreetype.so.6 (0x00007f099dc8f000)
        libgraphite2.so.3 => /lib/x86_64-linux-gnu/libgraphite2.so.3 (0x00007f099e5ed000)
        libicudata.so.72 => /lib/x86_64-linux-gnu/libicudata.so.72 (0x00007f099b400000)
        libXau.so.6 => /lib/x86_64-linux-gnu/libXau.so.6 (0x00007f09a3563000)
        libXdmcp.so.6 => /lib/x86_64-linux-gnu/libXdmcp.so.6 (0x00007f09a23a4000)
        libnghttp2.so.14 => /lib/x86_64-linux-gnu/libnghttp2.so.14 (0x00007f099e5c2000)
        libidn2.so.0 => /lib/x86_64-linux-gnu/libidn2.so.0 (0x00007f099eba1000)
        librtmp.so.1 => /lib/x86_64-linux-gnu/librtmp.so.1 (0x00007f099d5e1000)
        libssh.so.4 => /lib/x86_64-linux-gnu/libssh.so.4 (0x00007f099b390000)
        libpsl.so.5 => /lib/x86_64-linux-gnu/libpsl.so.5 (0x00007f099f56e000)
        libssl.so.3 => /lib/x86_64-linux-gnu/libssl.so.3 (0x00007f099b2eb000)
        libgssapi_krb5.so.2 => /lib/x86_64-linux-gnu/libgssapi_krb5.so.2 (0x00007f099b297000)
        libldap.so.2 => /lib/x86_64-linux-gnu/libldap.so.2 (0x00007f099b239000)
        liblber.so.2 => /lib/x86_64-linux-gnu/liblber.so.2 (0x00007f09a0d33000)
        libbrotlidec.so.1 => /lib/x86_64-linux-gnu/libbrotlidec.so.1 (0x00007f09a04d8000)
        libltdl.so.7 => /lib/x86_64-linux-gnu/libltdl.so.7 (0x00007f099d5d6000)
        libsnappy.so.1 => /lib/x86_64-linux-gnu/libsnappy.so.1 (0x00007f099d5cb000)
        libarpack.so.2 => /lib/x86_64-linux-gnu/libarpack.so.2 (0x00007f099b1ee000)
        libsuperlu.so.5 => /lib/x86_64-linux-gnu/libsuperlu.so.5 (0x00007f099b17c000)
        libcurl-gnutls.so.4 => /lib/x86_64-linux-gnu/libcurl-gnutls.so.4 (0x00007f099b0d4000)
        libx265.so.199 => /lib/x86_64-linux-gnu/libx265.so.199 (0x00007f099a000000)
        libde265.so.0 => /lib/x86_64-linux-gnu/libde265.so.0 (0x00007f099b078000)
        libdav1d.so.6 => /lib/x86_64-linux-gnu/libdav1d.so.6 (0x00007f0999e37000)
        libaom.so.3 => /lib/x86_64-linux-gnu/libaom.so.3 (0x00007f0999800000)
        libfontconfig.so.1 => /lib/x86_64-linux-gnu/libfontconfig.so.1 (0x00007f099b02a000)
        liblcms2.so.2 => /lib/x86_64-linux-gnu/liblcms2.so.2 (0x00007f099afc7000)
        libnss3.so => /lib/x86_64-linux-gnu/libnss3.so (0x00007f09996d2000)
        libsmime3.so => /lib/x86_64-linux-gnu/libsmime3.so (0x00007f099d5a3000)
        libplc4.so => /lib/x86_64-linux-gnu/libplc4.so (0x00007f099f951000)
        libnspr4.so => /lib/x86_64-linux-gnu/libnspr4.so (0x00007f099af87000)
        libhdf5_serial_hl.so.100 => /lib/x86_64-linux-gnu/libhdf5_serial_hl.so.100 (0x00007f099d1dc000)
        libbz2.so.1.0 => /lib/x86_64-linux-gnu/libbz2.so.1.0 (0x00007f099af74000)
        libsz.so.2 => /lib/x86_64-linux-gnu/libsz.so.2 (0x00007f099eb9c000)
        libminizip.so.1 => /lib/x86_64-linux-gnu/libminizip.so.1 (0x00007f0999400000)
        liburiparser.so.1 => /lib/x86_64-linux-gnu/liburiparser.so.1 (0x00007f0999e20000)
        libfyut.so.0 => /lib/x86_64-linux-gnu/libfyut.so.0 (0x00007f099af69000)
        libfygm.so.0 => /lib/x86_64-linux-gnu/libfygm.so.0 (0x00007f099d1d3000)
        librttopo.so.1 => /lib/x86_64-linux-gnu/librttopo.so.1 (0x00007f0999daf000)
        libresolv.so.2 => /lib/x86_64-linux-gnu/libresolv.so.2 (0x00007f0999d9c000)
        libtirpc.so.3 => /lib/x86_64-linux-gnu/libtirpc.so.3 (0x00007f09996a5000)
        libgeos.so.3.11.1 => /lib/x86_64-linux-gnu/libgeos.so.3.11.1 (0x00007f0999000000)
        libbsd.so.0 => /lib/x86_64-linux-gnu/libbsd.so.0 (0x00007f0999d87000)
        libunistring.so.2 => /lib/x86_64-linux-gnu/libunistring.so.2 (0x00007f099924c000)
        libgnutls.so.30 => /lib/x86_64-linux-gnu/libgnutls.so.30 (0x00007f0998e05000)
        libhogweed.so.6 => /lib/x86_64-linux-gnu/libhogweed.so.6 (0x00007f099965d000)
        libnettle.so.8 => /lib/x86_64-linux-gnu/libnettle.so.8 (0x00007f099960d000)
        libgmp.so.10 => /lib/x86_64-linux-gnu/libgmp.so.10 (0x00007f0998d82000)
        libkrb5.so.3 => /lib/x86_64-linux-gnu/libkrb5.so.3 (0x00007f0998cb9000)
        libk5crypto.so.3 => /lib/x86_64-linux-gnu/libk5crypto.so.3 (0x00007f0999220000)
        libcom_err.so.2 => /lib/x86_64-linux-gnu/libcom_err.so.2 (0x00007f0999d81000)
        libkrb5support.so.0 => /lib/x86_64-linux-gnu/libkrb5support.so.0 (0x00007f0998cac000)
        libsasl2.so.2 => /lib/x86_64-linux-gnu/libsasl2.so.2 (0x00007f0998c91000)
        libbrotlicommon.so.1 => /lib/x86_64-linux-gnu/libbrotlicommon.so.1 (0x00007f0998c6e000)
        libnuma.so.1 => /lib/x86_64-linux-gnu/libnuma.so.1 (0x00007f0998c61000)
        libnssutil3.so => /lib/x86_64-linux-gnu/libnssutil3.so (0x00007f0998c30000)
        libplds4.so => /lib/x86_64-linux-gnu/libplds4.so (0x00007f099dc8a000)
        libaec.so.0 => /lib/x86_64-linux-gnu/libaec.so.0 (0x00007f0998c27000)
        libmd.so.0 => /lib/x86_64-linux-gnu/libmd.so.0 (0x00007f0998c1a000)
        libp11-kit.so.0 => /lib/x86_64-linux-gnu/libp11-kit.so.0 (0x00007f0998add000)
        libtasn1.so.6 => /lib/x86_64-linux-gnu/libtasn1.so.6 (0x00007f0998ac6000)
        libkeyutils.so.1 => /lib/x86_64-linux-gnu/libkeyutils.so.1 (0x00007f0998abf000)
        libffi.so.8 => /lib/x86_64-linux-gnu/libffi.so.8 (0x00007f0998ab4000)
```