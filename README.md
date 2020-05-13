# GameProject

Setup on Windows: 
- `git clone https://github.com/Microsoft/vcpkg.git`
- `cd vcpkg`
- `.\bootstrap-vcpkg.bat`

Then get in to an administrator powershell and execute the following:
- `.\vcpkg integrate install`

Then add the printed path to clion->settings->cmake->cmake_options e.g. -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake
`.\vcpkg install freeglut glew glm gtest duktape` for the 32-bit
`.\vcpkg install --triplet x64-windows freeglut glew glm gtest duktape` to also enable the 64 bit
Installing both is suggested.

