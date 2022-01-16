cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(tinyRenderer)

add_executable(tinyRenderer
	main.cpp	
	lesson1/lesson1.cpp
)

add_subdirectory("${CMAKE_SOURCE_DIR}/thirdParty/glm")
add_subdirectory("${CMAKE_SOURCE_DIR}/thirdParty/glad")
add_subdirectory("${CMAKE_SOURCE_DIR}/thirdParty/canvas")
	
include_directories(
	"${CMAKE_SOURCE_DIR}/thirdParty/glfw/include"
	"${CMAKE_SOURCE_DIR}/lesson1"
	"${CMAKE_SOURCE_DIR}/thirdParty/glad/include"
	"${CMAKE_SOURCE_DIR}/thirdParty/glm"
	"${CMAKE_SOURCE_DIR}/thirdParty/canvas/include"
)

target_link_libraries(tinyRenderer 
	PRIVATE
	canvas
)
