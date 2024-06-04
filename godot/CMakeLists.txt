
include(FetchContent)

FetchContent_Declare(
		GDExtension
		GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
		GIT_TAG godot-4.2.2-stable
)

FetchContent_MakeAvailable(GDExtension)

# Automatically pick up files added to src
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Add a dynamic library called godoctopus - this builds godoctopus.dll
add_library(godot_navmesh SHARED ${SOURCES})
target_link_libraries(godot_navmesh PRIVATE godot::cpp navmesh)
target_include_directories(godot_navmesh PRIVATE src)

install (TARGETS godot_navmesh EXPORT ${PROJECT_NAME}-export)
