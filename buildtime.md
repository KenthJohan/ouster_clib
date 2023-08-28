## Build time for ouster_clib
```bash
$ time bake rebuild ouster_clib
[rebuild] package ouster_clib => 'ouster_clib'
[     9%] client_config.c
[    18%] meta.c
[    27%] json.c
[    36%] lidar_column.c
[    45%] mat.c
[    54%] net.c
[    63%] lidar_header.c
[    72%] os_file.c
[    81%] log.c
[    90%] client.c
[   100%] lidar_context.c

real    0m0.168s
user    0m0.141s
sys     0m0.027s
```



## Build time for ouster_client
https://github.com/KenthJohan/ouster_example1
```
$ time bake rebuild ouster_client
[rebuild] package ouster_client => 'ouster_client'
[     9%] buffered_udp_source.cpp
[    18%] client.cpp
[    27%] netcompat.cpp
[    36%] types.cpp
[    45%] sensor_http_imp.cpp
[    54%] lidar_scan.cpp
[    63%] image_processing.cpp
[    72%] parsing.cpp
[    81%] sensor_http.cpp
[    90%] logging.cpp
[   100%] sensor_tcp_imp.cpp

real    0m29.441s
user    0m27.908s
sys     0m1.449s
```