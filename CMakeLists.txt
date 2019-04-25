cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(Face-deformation-base-on-TPS)

find_package(opencv REQUIRED)

include_directories(
	${CMAKE_SOURCE_DIR}/libfacedetect
	${OpenCV_INCLUDE_DIRS}
)
MESSAGE(STATUS ${OpenCV_LIBS})
link_directories(
	${CMAKE_SOURCE_DIR}/libfacedetect
)

file(GLOB source_src src/*.cpp src/*.h)

add_executable(Face-deformation-base-on-TPS
	${source_src}
)

target_link_libraries(Face-deformation-base-on-TPS 
	libfacedetect-x64
	${OpenCV_LIBS}
)
