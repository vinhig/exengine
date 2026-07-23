# exengine, a C 3D game engine.

*Fork of [solenum/exengine](https://github.com/solenum/exengine)*

### What exactly is exengine?

exengine is a 3d engine that takes a slightly different approach than other libraries and engines do, in that it's a code-base you include directly into your own. Rather than using it as a static/shared library.

This approach allows easy and direct access to the engine back-end should you want to make modifications to suit your specific needs, so think of it as more of a template.

Assuming you don't want to set up your own build system (I can't blame you), you can clone the repo and use the existing build system and file structure as a starting template. It compiles on Linux and Windows. It *might* compile and run on MacOS, but this isn't [directly supported any more.](https://news.ycombinator.com/item?id=17231593)

**The contents of source/main.c and source/game.c are only supplied as examples to showcase how one might use the engine.**

### What are the features?

* *Simple* and small
* A straight-forward C codebase
* A forward renderer
* Point lights with smooth shadow mapping
* Normal & specular mapping
* Half-kernel SSAO
* IQM model loading
* 3D model animation
* Scene manager
* Instancing
* Polygon soup collision detection
* Smooth collision response
* Various cameras
* Dear ImGui integration
* MSDF text rendering

### (External) Depends

* A C23 compiler, preferably gcc. Clang and others should also work
* OpenGL 3.3+
* CMake 3.22+
* SDL3 (bundled as git submodule, built from source)
* Linux build dependencies: build-essential, cmake, ninja, X11/Wayland/PulseAudio/PipeWire/Mesa development packages

### Credits

* [SDL](https://github.com/libsdl-org/SDL)
* [MojoAL](https://hg.icculus.org/icculus/mojoAL/)
* [PhysFS](https://icculus.org/physfs/)
* [STB](https://github.com/nothings/stb)
* [Linmath](https://github.com/datenwolf/linmath.h)
* [IQM](http://sauerbraten.org/iqm/)
* [GLAD](https://github.com/Dav1dde/glad)

### Getting Started

#### Documentation

The current method of documentation is code comments, every engine header has extensive documentation as to its purpose and how one might use it. Better documentation will come in the near future.

#### Using & Compiling

Simply clone the repository (with submodules) and install the required libraries and compilers listed below for the OS you are using.

**This only applies to those wanting to use the current build system and game code as a starting template, and not those wanting to include exengine into an existing project or build system.**


----
### Linux

```
sudo apt update
sudo apt install build-essential cmake ninja-build \
  libpipewire-0.3-dev libwayland-dev libdecor-0-dev \
  libx11-dev libxext-dev libxrandr-dev libxcursor-dev \
  libxfixes-dev libxi-dev libxkbcommon-dev libdrm-dev \
  libgbm-dev libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev \
  libasound2-dev libpulse-dev libdbus-1-dev libudev-dev

git clone --recursive https://github.com/exezin/exengine
cd exengine
cmake -B build -G "Ninja Multi-Config"
cmake --build build --config Release
cd build && sh ../data.sh
```

The resulting binary will be in `build/Release/`.

----
### Windows

Cross-compile from Linux using MinGW:

```
sudo apt update
sudo apt install build-essential cmake ninja-build \
  g++-mingw-w64-x86-64 gcc-mingw-w64-x86-64 zip

git clone --recursive https://github.com/exezin/exengine
cd exengine
cmake -B build -G "Ninja Multi-Config" -DCMAKE_TOOLCHAIN_FILE=toolchain-mingw64.cmake
cmake --build build --config Release
cd build && sh ../data.sh
```

The resulting `.exe` will be in `build/Release/`.

You can also use the [Linux subsystem for Windows 10](https://docs.microsoft.com/en-us/windows/wsl/install-win10) to run the above steps natively.

----
### Stand-alone (*recommended*)

Alternatively you can drop the exengine/ directory right into your own project and use it with your own build system.  This method obviously takes more work and time.

----
### Gallery
![scrot](http://i.imgur.com/4NGlapU.png)
![scrot](https://i.imgur.com/vTKB3T8.png)
![scrot](http://i.imgur.com/H1pMBXI.png)
![scrot](https://i.imgur.com/n6FcPau.png)
![scrot](https://i.imgur.com/t7CcuiK.png)
![scrot](https://i.imgur.com/qesG2md.png)
