add_library(canvas
	STATIC
	"canvas.cpp"
)

configure_file(configuration/root_directory.h.in "${CMAKE_BINARY_DIR}/canvas/configuration/root_directory.h")

set(GLFW_LIBRARIES "${PROJECT_SOURCE_DIR}/thirdParty/glfw/lib-vc2019/glfw3.lib")

include_directories(canvas
	PRIVATE
	"${PROJECT_SOURCE_DIR}/thirdParty/glfw/include")
	"${PROJECT_SOURCE_DIR}/thirdParty/glad/include"
	"${CMAKE_BINARY_DIR}/canvas/configuration"
	PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(canvas 
	PRIVATE
	"${GLFW_LIBRARIES}"
	glm
	glad
)
