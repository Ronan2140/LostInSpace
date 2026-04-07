cmake_minimum_required(VERSION 3.20)
project(LostInSpace VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Eigen3 REQUIRED)

set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/externals/imgui")
set(IMGUI_SFML_DIR "${CMAKE_SOURCE_DIR}/externals/imgui-SFML")

set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_SFML_DIR}/imgui-SFML.cpp
)

set(PROJECT_SOURCES
        src/StarTrackerUI.cpp
        src/StarMatcher.cpp
        src/CameraSimulator.cpp
        src/StarCatalogue.cpp
        src/TriangleDatabase.cpp
        src/main.cpp
        include/Config.hpp
        include/Timer.hpp
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${IMGUI_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        "${IMGUI_DIR}"
        "${IMGUI_SFML_DIR}"
        "/usr/include/eigen3"
)

target_link_libraries(${PROJECT_NAME}
        sfml-graphics
        sfml-window
        sfml-system
        OpenGL::GL
        Eigen3::Eigen
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/data"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/data"
)