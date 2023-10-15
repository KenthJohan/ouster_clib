This is a non official SDK for Ouster LiDAR sensors.<br>
The ouster_clib is meant to be simplistic and use zero dependencies. Easy to debug, modify and extend. 
Dependency on `libcurl` is optional and is only used to download meta file.
<br><br>
The official SDK can be found at https://github.com/ouster-lidar/ouster_example. <br>

## Features
* Simple and portable with no dependencies [C99 API](https://kenthjohan.github.io/ouster_clib/group__c.html)
* Short build time
* C++ compatible
* Easy to integrate with [OpenCV](examples/opencv_example/src/opencv_example.cpp) or Eigen
* Debug friendly
* Meta file parser
* Meta file downloader is optional. Requires dependency libcurl.
* Destagger
* LUT table for XYZ support
* Completes a frame exactly at the last packet
* Memory requirement depends on field of view
* No hidden state
* No Windows support

## Supported devices
* OS1-16
* OS1-128

## Documentation
- [All Documentation](https://kenthjohan.github.io/ouster_clib/md_docs_Docs.html)


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

