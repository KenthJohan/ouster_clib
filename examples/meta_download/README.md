# meta_download

This downloads the metafile from Ouster LIDAR.

## ldd

```bash
$ ldd examples/meta_download/bin/x64-Linux-debug/meta_download 
        linux-vdso.so.1 (0x00007ffd835dc000)
        libcurl.so.4 => /lib/x86_64-linux-gnu/libcurl.so.4 (0x00007f471ae06000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f471ac00000)
        libnghttp2.so.14 => /lib/x86_64-linux-gnu/libnghttp2.so.14 (0x00007f471abd5000)
        libidn2.so.0 => /lib/x86_64-linux-gnu/libidn2.so.0 (0x00007f471abb4000)
        librtmp.so.1 => /lib/x86_64-linux-gnu/librtmp.so.1 (0x00007f471ab95000)
        libssh.so.4 => /lib/x86_64-linux-gnu/libssh.so.4 (0x00007f471ab23000)
        libpsl.so.5 => /lib/x86_64-linux-gnu/libpsl.so.5 (0x00007f471ab0f000)
        libssl.so.3 => /lib/x86_64-linux-gnu/libssl.so.3 (0x00007f471aa6a000)
        libcrypto.so.3 => /lib/x86_64-linux-gnu/libcrypto.so.3 (0x00007f471a600000)
        libgssapi_krb5.so.2 => /lib/x86_64-linux-gnu/libgssapi_krb5.so.2 (0x00007f471a5ac000)
        libldap.so.2 => /lib/x86_64-linux-gnu/libldap.so.2 (0x00007f471a54e000)
        liblber.so.2 => /lib/x86_64-linux-gnu/liblber.so.2 (0x00007f471aa58000)
        libzstd.so.1 => /lib/x86_64-linux-gnu/libzstd.so.1 (0x00007f471a49a000)
        libbrotlidec.so.1 => /lib/x86_64-linux-gnu/libbrotlidec.so.1 (0x00007f471a48d000)
        libz.so.1 => /lib/x86_64-linux-gnu/libz.so.1 (0x00007f471a46f000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f471aed4000)
        libunistring.so.2 => /lib/x86_64-linux-gnu/libunistring.so.2 (0x00007f471a2bb000)
        libgnutls.so.30 => /lib/x86_64-linux-gnu/libgnutls.so.30 (0x00007f471a0c0000)
        libhogweed.so.6 => /lib/x86_64-linux-gnu/libhogweed.so.6 (0x00007f471a078000)
        libnettle.so.8 => /lib/x86_64-linux-gnu/libnettle.so.8 (0x00007f471a028000)
        libgmp.so.10 => /lib/x86_64-linux-gnu/libgmp.so.10 (0x00007f4719fa5000)
        libkrb5.so.3 => /lib/x86_64-linux-gnu/libkrb5.so.3 (0x00007f4719edc000)
        libk5crypto.so.3 => /lib/x86_64-linux-gnu/libk5crypto.so.3 (0x00007f4719eb0000)
        libcom_err.so.2 => /lib/x86_64-linux-gnu/libcom_err.so.2 (0x00007f4719eaa000)
        libkrb5support.so.0 => /lib/x86_64-linux-gnu/libkrb5support.so.0 (0x00007f4719e9d000)
        libsasl2.so.2 => /lib/x86_64-linux-gnu/libsasl2.so.2 (0x00007f4719e82000)
        libbrotlicommon.so.1 => /lib/x86_64-linux-gnu/libbrotlicommon.so.1 (0x00007f4719e5f000)
        libp11-kit.so.0 => /lib/x86_64-linux-gnu/libp11-kit.so.0 (0x00007f4719d22000)
        libtasn1.so.6 => /lib/x86_64-linux-gnu/libtasn1.so.6 (0x00007f4719d0b000)
        libkeyutils.so.1 => /lib/x86_64-linux-gnu/libkeyutils.so.1 (0x00007f4719d04000)
        libresolv.so.2 => /lib/x86_64-linux-gnu/libresolv.so.2 (0x00007f4719cf1000)
        libffi.so.8 => /lib/x86_64-linux-gnu/libffi.so.8 (0x00007f4719ce6000)
```