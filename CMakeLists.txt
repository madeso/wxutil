cmake_minimum_required(VERSION 3.1.0)
set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(wxutil)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    "${PROJECT_SOURCE_DIR}/cmake-modules")

if(${APPLE})
  # disabling http://stackoverflow.com/questions/32626171/xcode-7-how-to-suppress-warning-overrides-a-member-function-but-is-not-marked
  # that apple-clang spews out for wx headers
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-inconsistent-missing-override")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
endif()

# compiler flags
if(${MSVC})
	add_definitions(/D _CRT_SECURE_NO_WARNINGS)

	# character set to Not Set=ASCII/SBCS
  # http://public.kitware.com/Bug/view.php?id=12189
	add_definitions(/D _SBCS)
endif()

# force visual studio and the like to only have debug and release builds
if(CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_CONFIGURATION_TYPES Debug Release)
	set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Reset the configurations to what we need" FORCE)
endif()

set(src demo.cc selectcolor.cc selectcolor.h)
source_group("" FILES ${src})

find_package(wxWidgets COMPONENTS core base gl)
include( "${wxWidgets_USE_FILE}" )
add_executable(demo WIN32 MACOSX_BUNDLE ${src})
target_link_libraries(demo
                      ${wxWidgets_LIBRARIES}
                      )

add_executable(connex
							 connex/connex.cpp
							 connex/connex_example.cpp
							 connex/connex_example.h
							 connex/connex.h
							 connex/connexnode.cpp
							 connex/connexnode.h
							 connex/connexspace.cpp
							 connex/connexspace.h
)

target_link_libraries(connex
	${wxWidgets_LIBRARIES}
)

