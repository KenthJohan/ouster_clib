# This is non official Ouster clib SDK
Inspired from https://github.com/ouster-lidar/ouster_example

## Building
Install https://github.com/SanderMertens/bake<br>
```bash
$ bake ouster_clib
```
### Running examples
```bash
$ bake run examples/client_example
```

```bash
$ bake run examples/opencv_example
```

## Examples
* Minimal example
* OpenCV example

## Features
* C++ compatable
* Meta file parser
* Adaptive field size. No waste of black pixels.
* Build time of nosdk **ouster_clib** : 0.168s. Build time official of **ouster_client** : 29.441s.
* No hidden state
* No Windows support

## TODO
* Automatic meta file creation
* HTTP client
* Support all format profiles
* OS layer



