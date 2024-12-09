# CMakeList.txt: Breakout 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

# 设置包含目录 
include_directories(./include)

# 将源代码添加到此项目的可执行文件(# 添加可执行文件)
aux_source_directory(./src DIR_BASE_SRCS)
add_executable(Breakout ${DIR_BASE_SRCS})

# 链接库 
target_link_libraries(Breakout ${GLFW3_LIBRARY} glad stb_image irrKlang freetype)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Breakout PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
