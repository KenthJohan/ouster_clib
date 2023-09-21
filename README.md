# Ouster Non Official SDK
Simplistic small C library for Ouster LiDAR sensors.<br>
The official SDK can be found at https://github.com/ouster-lidar/ouster_example.
<br><br>
The `ouster_clib` is meant to be simplistic and use zero dependencies. Easy to debug, modify and extend.
`libcurl` is currently used to get meta data but its not required to receive LiDAR frames.




## Examples
* Minimal example<br>
This example receive frames from sensor.
* OpenCV example<br>
This example receive frames from sensor and converts `ouster_mat_t` to `cv::Mat` and then draws it on screen.


## Features
* C++ compatable
* Debug friendly library
* Very easy to integrate with OpenCV or Eigen
* Meta file parser and creation
* Destagger
* Adaptive field size. No waste of black pixels.
* Build time of nosdk `ouster_clib` = `0.168s`. <br>
Build time of official `ouster_client` = `29.441s`.
* No hidden state
* No Windows support

## TODO
* HTTP client configure support
* Support all format profiles
* OS layer abstraction
* LUT table for XYZ support
* Maybe remove `libcurl` dependency






## Building

### Requirement
```bash
sudo apt update
sudo apt install git build-essential libcurl4-openssl-dev
```



Install https://github.com/SanderMertens/bake<br>
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
bake platform
bake ouster_clib
```



### Build tools
```bash
bake tools
```


### Build examples
```bash
bake examples
```


### VS code
```bash
sudo snap install code --classic
```




## Running examples
### Requirement
```bash
sudo apt update
sudo apt install git build-essential libcurl4-openssl-dev  libglfw3-dev libglew-dev libopencv-dev
```

```bash
bake run examples/client_example
```

```bash
bake run examples/opencv_example
```







