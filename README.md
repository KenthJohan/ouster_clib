This is a non official SDK for Ouster LiDAR sensors.<br>
The ouster_clib is meant to be simplistic and uses zero dependencies. Easy to debug, modify and extend. 
Don't want to build lib? then drop `ouster_clib.h` and `ouster_clib.c` in any project.
<br><br>
The official SDK can be found at https://github.com/ouster-lidar/ouster_example. <br>

## Features
* Simple and portable with no dependencies [C99 API](https://kenthjohan.github.io/ouster_clib/group__c.html)
* Short build time
* C++ compatible
* Easy to integrate with [OpenCV](examples/opencv_example/src/opencv_example.cpp) or Eigen
* Debug friendly
* Meta file parser and downloader
* Generic destagger
* LUT table for converting image to XYZ pointcloud
* Completes a frame exactly at the last packet
* Memory requirement depends on field of view

## Supported devices
I have only tested on these sensors but it should work an all others as Ouster sensor uses common packet format.
* OS1-16
* OS1-128

## Documentation
- [All Documentation](https://kenthjohan.github.io/ouster_clib/md_docs_Docs.html)


## TODO
* Add cmake build system
* Add meson build system
* HTTP client configure support
* Support all format profiles
* OS layer abstraction


## Build systems supported
* [bake](https://github.com/SanderMertens/bake)



## Requirement
libcurl is optional.
```bash
sudo apt update
sudo apt install git
sudo apt install build-essential
```

## Requirement building with bake
```bash
cd ~/Downloads
git clone https://github.com/SanderMertens/bake
bake/setup.sh
cd ~/Downloads
git clone https://github.com/KenthJohan/ouster_clib/
bake ouster_clib
```

## VS code
```bash
sudo snap install code --classic
```
