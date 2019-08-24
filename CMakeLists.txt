#CMakeLists.txt
cmake_minimum_required(VERSION 3.1)
project(Snake)


find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIRS})

find_package(Boost REQUIRED COMPONENTS thread system)
include_directories(${Boost_INCLUDE_DIR})

link_directories(/usr/local/lib)^

set(MAINSRC
  src/snake.cpp
  src/main.cpp
)

set(MAINLIBS
  application
  ${Boost_LIBRARIES}
)

add_executable(Snake ${MAINSRC})
target_link_libraries(Snake ${MAINLIBS})