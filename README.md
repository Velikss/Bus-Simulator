# GameProject

Setup on Windows: 
- `git clone https://github.com/Microsoft/vcpkg.git`
- `cd vcpkg`
- `.\bootstrap-vcpkg.bat`

Then add the printed path to clion->settings->cmake->cmake_options 

`-DCMAKE_TOOLCHAIN_FILE=%vcpkgdir%/scripts/buildsystems/vcpkg.cmake`

Then install all packages:
`vcpkgdir%\vcpkg install glew:x64-windows glew:x86-windows glm:x64-windows glm:x86-windows gtest:x64-windows gtest:x86-windows openssl:x64-windows openssl:x64-windows-static openssl:x86-windowsopenssl:x86-windows-static`

Download the VulkanSDK, and set the following environment variables:
`VULKAN_SDK: %VULKANDIR/VERSION%`
`VK_SDK_PATH: %VULKANDIR/VERSION%`
`VULKAN_INCLUDE_DIR: %VULKANDIR/VERSION%/Include`
`VULKAN_LIBRARY: %VULKANDIR/VERSION%/Lib\vulkan-1.lib`
