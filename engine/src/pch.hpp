#pragma once

#define GLFW_INCLUDE_VULKAN                 // We want to use GLFW with Vulkan
#define GLM_FORCE_RADIANS                   // Force GLM to use radians everywhere
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES  // Force GLM to use aligned types by default
#define GLM_FORCE_DEPTH_ZERO_TO_ONE         // Vulkan uses a depth range from 0 to 1, so we need GLM to do the same

#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <map>
#include <stack>
#include <cassert>
#include <set>
#include <array>

#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include <vendor/stb_image.h>
#include <vendor/stb_font_arial_50_usascii.inl>
#include <vendor/tiny_obj_loader.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

typedef std::string string;
typedef unsigned int uint;
typedef long long int64;
typedef unsigned long long uint64;
typedef unsigned char byte;
