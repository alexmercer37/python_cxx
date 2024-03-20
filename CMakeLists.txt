cmake_minimum_required(VERSION 3.16)

include_directories(.)
project(v2)

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(OpenCV REQUIRED)
find_package(k4a REQUIRED)
find_package(PythonLibs 3 REQUIRED)


include_directories(${PYTHON_INCLUDE_DIR})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${NUMPY_INCLUDES})


set(CMAKE_CXX_STANDARD 14)

add_executable(v2 src/main.cpp src/camera.cpp)
# add_executable(v2 src/camera.cpp src/tcp.cpp)



target_include_directories(${PROJECT_NAME} PRIVATE ${Python3_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${Python3_LIBRARIES})
target_link_libraries(v2 k4a::k4a)
target_link_libraries(v2 ${TORCH_LIBRARIES} ${OpenCV_LIBS})

