cmake_minimum_required(VERSION 2.6)

project (connex)


SET(CMAKE_CXX_WARNING_LEVEL 4)

IF(CMAKE_COMPILER_IS_GNUCXX)
	SET(CMAKE_C_FLAGS_DEBUG "-g -O3")
	SET(CMAKE_CXX_FLAGS_DEBUG "-g -O3")
	SET(CMAKE_CXX_FLAGS_DISTRIBUTION "-g -O3")
	SET(CMAKE_C_FLAGS_DISTRIBUTION "-g -O3")
	SET(CMAKE_CXX_FLAGS "-g")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


# Various modules used for library path detections.
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

FIND_PACKAGE(wxWidgets REQUIRED mono)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${wxWidgets_CXX_FLAGS}")
ADD_DEFINITIONS( ${wxWidgets_DEFINITIONS} )

include_directories(
	${wxWidgets_INCLUDE_DIRS}
)

add_executable(connex
	connex.cpp
	connex_example.cpp
	connex_example.h
	connex.h
	connexnode.cpp
	connexnode.h
	connexspace.cpp
	connexspace.h
)

target_link_libraries(connex
	${wxWidgets_LIBRARIES}
)
