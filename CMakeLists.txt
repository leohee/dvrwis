cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)

project(streamer VERSION 0.1.0 LANGUAGES CXX C)

set(TARGET_FILE "wis-streamer")


include_directories(
	"wis-streamer"
	"wis-streamer/AACEncoder"
	"wis-streamer/AMREncoder"
)

file(GLOB_RECURSE SRC_FILES "wis-streamer/*.c*")

set(LIB_LIVE "live")
add_subdirectory("libs/liblive")

set(LIB_NETRA "netra")
add_subdirectory("libs/libnetra")

ADD_EXECUTABLE(${TARGET_FILE} ${SRC_FILES})

TARGET_LINK_LIBRARIES(${TARGET_FILE} 
	m
	${LIB_LIVE}
	${LIB_NETRA}
)

