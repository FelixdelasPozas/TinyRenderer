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

![zbuffer]()

Result of the ambient occlusion pass.

![ambient]()

Light depth pass to compute self-shadowing.

![lightdepth]()

Final render: diffuse, ambient and specular lightning with normal mapping.

![render]()

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
