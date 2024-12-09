# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("LearnOpenGL")

# 查找外部库
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules/cmake/")
find_package(GLFW3 REQUIRED)

# 添加库搜索路径  
link_directories(${CMAKE_SOURCE_DIR}/modules/irrKlang-64bit-1.6.0/lib/Winx64-visualStudio)
link_directories(${CMAKE_SOURCE_DIR}/modules/freetype-windows-binaries-2.13.3/release\ dll/win64)#空格需要转义，或者整体用引号，否则cmake认为是两个字符串（路径）

# 添加自定义库 
add_library(glad ${CMAKE_SOURCE_DIR}/modules/glad4.6/src/glad.c)
add_library(stb_image ${CMAKE_SOURCE_DIR}/modules/stb_image/stb_image_wrap.cpp)

# 设置包含目录
include_directories(${GLFW3_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/modules/glad4.6/include)
include_directories(${CMAKE_SOURCE_DIR}/modules/stb_image)
include_directories(${CMAKE_SOURCE_DIR}/modules/glm-1.0.1)
include_directories(${CMAKE_SOURCE_DIR}/modules/irrKlang-64bit-1.6.0/include)
include_directories(${CMAKE_SOURCE_DIR}/modules/freetype-windows-binaries-2.13.3/include)

# 包含子项目。
add_subdirectory ("Breakout")
