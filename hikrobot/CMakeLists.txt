cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_CXX_STANDARD 17)
project(testcamera)

find_package(OpenCV REQUIRED)
find_package(Threads)

include_directories(./include ${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS} /opt/MVS/lib/64)

#链接动态静态库
add_executable(main main.cpp hik_camera.cpp)

target_link_libraries(main -lMvCameraControl -lpthread ${OpenCV_LIBS})

