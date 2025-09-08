cmake_minimum_required(VERSION 3.22)
project(modulix LANGUAGES CXX)

# variables
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(ENABLE_PCH ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# compiler flags
if(MSVC)
  add_compile_options(/W1 /favor:AMD64 /arch:AVX2 /openmp /MT)
elseif(APPLE)
  add_compile_options(-Wall -march=native)
else()
  add_compile_options(-Wall -march=native -fopenmp)
  add_link_options(-fopenmp)
endif()

# definitions
add_compile_definitions(PROJECT_DIRECTORY="${CMAKE_SOURCE_DIR}" DEVELOP)

# ignore warnings from external sources
file(GLOB_RECURSE SRC_LIBS CONFIGURE_DEPENDS libs/*.c libs/*.cpp libs/*.h libs/*.hpp)
if(MSVC)
  set_source_files_properties(${SRC_LIBS} PROPERTIES COMPILE_OPTIONS /w)
else()
  set_source_files_properties(${SRC_LIBS} PROPERTIES COMPILE_OPTIONS -w)
endif()

# ccache
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
endif()

# targets
add_executable(modulix)

# precompiled headers
if (ENABLE_PCH)
  set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)
  target_precompile_headers(modulix PRIVATE ${CMAKE_SOURCE_DIR}/apps/modulix/Precompiled.hpp)
endif()

# fmt
add_subdirectory(libs/fmt)
include_directories(libs/fmt/include)
target_link_libraries(modulix PRIVATE fmt::fmt)

# opencv
set(OpenCV_STATIC ON)
find_package(OpenCV REQUIRED HINTS ${OPENCV_DIR} ${CMAKE_SOURCE_DIR}/libs/opencv_install)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(modulix PRIVATE ${OpenCV_LIBS})

# onnxruntime
set(ONNXRUNTIME_DIR "${CMAKE_SOURCE_DIR}/libs/onnxruntime_install" CACHE INTERNAL "ONNX runtime dir")
if (MSVC)
    set(ONNXRUNTIME_LIBS ${ONNXRUNTIME_DIR}/lib/onnxruntime.lib CACHE INTERNAL "ONNX runtime lib")
else()
    set(ONNXRUNTIME_LIBS ${ONNXRUNTIME_DIR}/lib/libonnxruntime.so CACHE INTERNAL "ONNX runtime lib")
endif()
include_directories(${ONNXRUNTIME_DIR}/include)
target_link_libraries(modulix PRIVATE ${ONNXRUNTIME_LIBS})

# custom
include_directories(libs)
include_directories(src)

# modulix sources
file(GLOB_RECURSE SRC_MODULIX CONFIGURE_DEPENDS
 src/Engine/*.cpp
 src/Log/*.cpp
 src/Modules/*.cpp
 src/Onnx/*.cpp
 src/Utils/*.cpp
)

# gui soruces
file(GLOB_RECURSE SRC_GUI CONFIGURE_DEPENDS src/Gui/*.cpp)

# modulix app sources
file(GLOB_RECURSE SRC_MODULIX_APP CONFIGURE_DEPENDS apps/modulix/*.cpp)
target_sources(modulix PRIVATE ${SRC_MODULIX} ${SRC_MODULIX_APP})



