# Minimal example
This example receive frames from sensor.

## ldd
```bash
$ ldd examples/client_example/bin/x64-Linux-debug/client_example 
        linux-vdso.so.1 (0x00007fff35b9b000)
        libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007fc8b409e000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fc8b3e00000)
        /lib64/ld-linux-x86-64.so.2 (0x00007fc8b41b1000)
```