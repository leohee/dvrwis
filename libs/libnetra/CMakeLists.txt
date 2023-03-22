


FILE(GLOB SRC_NETRA 
	"*.c" 
)

add_library (${LIB_NETRA} STATIC ${SRC_NETRA})

target_include_directories(${LIB_NETRA} PUBLIC 
	"./" 
)

target_link_libraries(${LIB_NETRA})

