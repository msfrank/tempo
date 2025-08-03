Tempo - Utility libraries for the Zuri project
==============================================

Copyright (c) 2024, Michael Frank

Overview
--------

Tempo is a collection of utility libraries used by the Zuri project.

Prerequisites
-------------

Tempo requires the following dependencies to be installed on the system:
```
CMake version 3.27 or greater:      https://cmake.org
Conan version 2:                    https://conan.io
```

Tempo also depends on the package recipes from Timbre and expects that the recipes
are exported into the Conan2 package cache.

Quick Start
-----------

1. Navigate to the repository root.
1. Build and install Tempo using Conan:
  ```
conan create . --build=missing -s build_type=Release
  ```

This will build all Tempo artifacts in Release mode and install them in the Conan2
package cache.

Development
-----------

1. Navigate to the repository root.
1. Install the Tempo recipe into the Conan package cache:
  ```
conan install . --build=missing -s build_type=Debug
  ```
1. Apply the generated CMake preset:
  ```
cmake --preset conan-debug
  ```

The CMake build system will now be fully configured in the `build/Debug/` directory.

Building the Docker Image
-------------------------

The build system is not configured by default to build the docker image. In order to
enable the `build_docker_image` target the `-o '&:enable_docker_build=True'` option must
be specified when invoking `conan install`.

Licensing
---------

The software in this package is licensed under the terms of the BSD 3-clause license.
The text of the license is contained in the file `LICENSE.txt`.