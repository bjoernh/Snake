#CMakeLists.txt
cmake_minimum_required(VERSION 3.1)
project(Snake)

set(CMAKE_CXX_STANDARD 14)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter -DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=16")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin")

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIRS})

find_package(Boost REQUIRED COMPONENTS thread system)
include_directories(${Boost_INCLUDE_DIR})

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

find_package(ImageMagick COMPONENTS Magick++ REQUIRED)
include_directories(${ImageMagick_INCLUDE_DIRS})

include_directories(/home/pi/MatrixScreenServer/include)
link_directories(/home/pi/MatrixScreenServer/lib)

set(MAINSRC
  src/snake.cpp
  src/main.cpp
)

set(MAINLIBS
  libmatrixapp.so
  ${Boost_LIBRARIES}
  ${ImageMagick_LIBRARIES}
)

add_executable(Snake ${MAINSRC})
target_link_libraries(Snake ${MAINLIBS}) #-l flag

add_custom_target(run
    COMMAND Snake
    DEPENDS Snake
    WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
)
