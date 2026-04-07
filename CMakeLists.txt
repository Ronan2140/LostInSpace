cmake_minimum_required(VERSION 3.20)
project(LostInSpace VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Eigen3 REQUIRED)

add_executable(StarTracker
        src/main.cpp
        src/StarCatalogue.cpp
        src/CameraSimulator.cpp
        src/TriangleDatabse.cpp
        include/TriangleDatabse.hpp
        src/StarMatcher.cpp
        include/StarMatcher.hpp
        src/StarTrackerUi.cpp
        include/StarTrackerUi.hpp
)

target_include_directories(StarTracker PRIVATE
        include
        ${EIGEN3_INCLUDE_DIRS}
        /usr/include/eigen3
)

target_link_libraries(StarTracker Eigen3::Eigen)