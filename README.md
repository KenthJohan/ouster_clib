# Non official Ouster SDK
This library is a API for Ouster LiDARS.<br>
The official SDK API can be found at https://github.com/ouster-lidar/ouster_example.
<br><br>
This **ouster_clib** is meant to be simplistic and use zero dependencies.
The examples that uses this library will have dependencies like OpenCV. 





## Examples
* Minimal example
* OpenCV example

## Features
* C++ compatable
* Debug friendly library
* Very easy to integrate with OpenCV or Eigen
* Meta file parser and creation
* Adaptive field size. No waste of black pixels.
* Build time of nosdk **ouster_clib** : 0.168s. Build time official of **ouster_client** : 29.441s.
* No hidden state
* No Windows support

## TODO
* HTTP client configure support
* Support all format profiles
* OS layer abstraction
* LUT table for XYZ support
* Maybe remove libcurl dependency






## Building

### Requirement
```bash
sudo apt install git build-essential libcurl4-openssl-dev  libglfw3-dev libglew-dev
sudo apt update
sudo apt install libopencv-dev
```



Install https://github.com/SanderMertens/bake<br>
```bash
git clone https://github.com/SanderMertens/bake
mv bake bake_repo
bake_repo/setup.sh
rm -r bake_repo
```
### Build ouster_clib
```bash
git clone https://github.com/KenthJohan/ouster_clib/
cd ouster_clib
bake ouster_clib
```

### VS code
```bash
sudo snap install code --classic
```




## Running examples
```bash
bake run examples/client_example
```

```bash
bake run examples/opencv_example
```







