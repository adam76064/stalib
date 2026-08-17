cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(STALib VERSION 1.0.0 LANGUAGES CXX)

# ============================================================
# C++17 — required for modern language features
# ============================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ============================================================
# Qt 5 — internal image processing dependency
# ============================================================
find_package(Qt5 5.15 COMPONENTS Core Gui Xml REQUIRED)

# ============================================================
# Boost — internal header-only math and geometry dependency
# ============================================================
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# ============================================================
# pybind11
# ============================================================
find_package(pybind11 REQUIRED)

# ============================================================
# Compiler flags
# ============================================================
if(MSVC)
  add_compile_options(
    /W3
    /wd4267 /wd4244 /wd4305  # narrowing conversions (int <-> size_t)
    /wd4996                  # deprecated CRT functions
    /wd4819 /wd4018
    /bigobj
    /Zc:__cplusplus
    /EHsc
    /D_SECURE_SCL=0
    /D_USE_MATH_DEFINES      # M_PI on Windows
  )
  add_compile_definitions(NOMINMAX)
else()
  add_compile_options(-Wall -Wextra -Wno-unused-parameter -fvisibility=hidden)
endif()

# ============================================================
# Include search paths (STA internal headers)
# ============================================================
set(STALIB_INCLUDE_DIRS
  "${CMAKE_CURRENT_SOURCE_DIR}/lib"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/imageproc"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/foundation"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/math"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/dewarping"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/core"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/core/filters/fix_orientation"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/core/filters/page_split"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/core/filters/deskew"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/core/filters/select_content"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/core/filters/page_layout"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/core/filters/output"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/core/zones"
)

# ============================================================
# Collect sources
# ============================================================
file(GLOB_RECURSE STALIB_SOURCES
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/imageproc/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/foundation/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/math/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/math/adiff/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/math/spfit/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/dewarping/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/core/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/core/zones/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/core/filters/fix_orientation/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/core/filters/page_split/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/core/filters/deskew/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/core/filters/select_content/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/core/filters/page_layout/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/core/filters/output/*.cpp"
)

# ============================================================
# pybind11 extension — single module "stalib_cpp"
# ============================================================
pybind11_add_module(stalib_cpp
  ${STALIB_SOURCES}
  "${CMAKE_CURRENT_SOURCE_DIR}/bindings/image_utils.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/bindings/bind_fix_orientation.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/bindings/bind_page_split.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/bindings/bind_deskew.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/bindings/bind_select_content.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/bindings/bind_page_layout.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/bindings/bind_output.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/bindings/module.cpp"
)

target_include_directories(stalib_cpp PRIVATE
  ${STALIB_INCLUDE_DIRS}
  "${CMAKE_CURRENT_SOURCE_DIR}/bindings"
)

target_link_libraries(stalib_cpp PRIVATE
  Qt5::Core
  Qt5::Gui
  Qt5::Xml
)

# ============================================================
# Install — place the .pyd/.so next to the stalib Python package
# ============================================================
install(TARGETS stalib_cpp
  LIBRARY DESTINATION stalib
  RUNTIME DESTINATION stalib
)
