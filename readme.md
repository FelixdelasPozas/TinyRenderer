Tiny Software Renderer
======================

# Summary
- [Description](#description)
- [Compilation](#compilation-requirements)
- [Install](#install)
- [Screenshots](#screenshots)
- [Repository information](#repository-information)

# Description
My own implementation of the [tiny renderer](https://github.com/ssloy/tinyrenderer) course.

It contains no deviations from the course with the following exceptions:
* Use of OpenMP and some AVX vectorization (for the ambient occlusion pass) to accelerate the rendering process.
* Mutexes to avoid getting incorrect images.
* Code to read more complex Wavefront obj files with multiple sub-objects and materials. 

# Compilation requirements to build the program:
* cross-platform build system: [CMake](http://www.cmake.org/cmake/resources/software.html).
* compiler: [Mingw64](http://sourceforge.net/projects/mingw-w64/) on Windows or [gcc](http://gcc.gnu.org/) on Linux.

# Install
The only current option is build from source as binaries are not provided. 

# Screenshots

Some images from the renderer.

Scene z-buffer.

![zbufferpass](https://cloud.githubusercontent.com/assets/12167134/17668098/1d33a606-6308-11e6-81b8-8a1683fabf30.png)

Result of the ambient occlusion pass.

![ambient](https://cloud.githubusercontent.com/assets/12167134/17668100/1d4239dc-6308-11e6-854d-21559a07d6d2.png)

Light depth pass to compute self-shadowing.

![depthpass](https://cloud.githubusercontent.com/assets/12167134/17668099/1d3b1274-6308-11e6-8019-0dfe3f0fecf6.png)

Final render: diffuse, ambient and specular lightning with normal mapping.

![render](https://cloud.githubusercontent.com/assets/12167134/17668097/1d28486a-6308-11e6-912a-69b6bf6096c3.png)

# Repository information
**Version**: 1.0.0

**Status**: finished

**cloc statistics**

| Language                     |files          |blank        |comment           |code  |
|:-----------------------------|--------------:|------------:|-----------------:|-----:|
| C++                          |    6          |  452        |   210            | 1766 |
| C/C++ Header                 |    6          |  401        |   962            | 1220 |
| CMake                        |    1          |  9          |     3            |   35 |
| **Total**                    |   **13**      |  **862**    |   **1175**       |**3021**|
