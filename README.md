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
are exported into the conan2 package cache.

Quick Start
-----------

1. Navigate to the repository root.
1. Build and install Tempo using Conan:
  ```
conan create . --build=missing
  ```

Licensing
---------

The software in this package is licensed under the terms of the BSD 3-clause license.
The text of the license is contained in the file `LICENSE.txt`.