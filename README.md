# Ouster Non Official SDK
Simplistic small C library for Ouster LiDAR sensors.<br>
The official SDK can be found at https://github.com/ouster-lidar/ouster_example.
<br><br>
The `ouster_clib` is meant to be simplistic and use zero dependencies. Easy to debug, modify and extend.
`libcurl` is currently used to get meta data but its not required to receive LiDAR frames.


## Features
* C++ compatable
* Debug friendly library
* Very easy to integrate with OpenCV or Eigen
* Meta file parser and creation
* Destagger
* LUT table for XYZ support
* Adaptive field size. No waste of black pixels.
* Build time of nosdk `ouster_clib` = `0.168s`. <br>
Build time of official `ouster_client` = `29.441s`.
* No hidden state
* No Windows support

## TODO
* HTTP client configure support
* Support all format profiles
* OS layer abstraction
* Maybe remove `libcurl` dependency


## Build tools supported
* https://github.com/SanderMertens/bake









## Requirement

```bash
sudo apt update
sudo apt install git build-essential libcurl4-openssl-dev
```

### Install bake
Currently the only building tool supported is https://github.com/SanderMertens/bake
```bash
cd ~/Downloads
git clone https://github.com/SanderMertens/bake
bake/setup.sh
```

### Build ouster_clib
```bash
cd ~/Downloads
git clone https://github.com/KenthJohan/ouster_clib/
cd ouster_clib
bake ouster_clib
```






## Building





### Build all tools
```bash
bake tools
```


### Build all examples
```bash
bake examples
```


### VS code
```bash
sudo snap install code --classic
```




## Running examples


```bash
bake run examples/client_example -a <arg1>
bake run examples/opencv_example -a <arg1>
```






## ldd minimal example client_example

```bash
$ ldd examples/client_example/bin/x64-Linux-debug/client_example 
        linux-vdso.so.1 (0x00007fff35b9b000)
        libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007fc8b409e000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fc8b3e00000)
        /lib64/ld-linux-x86-64.so.2 (0x00007fc8b41b1000)
```
</details>


