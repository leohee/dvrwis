


FILE(GLOB SRC_LIVE 
	"groupsock/*.c*" 
	"liveMedia/*.c*" 
	"UsageEnvironment/*.c*" 
	"BasicUsageEnvironment/*.c*" 
)

add_library (${LIB_LIVE} STATIC ${SRC_LIVE})

target_include_directories(${LIB_LIVE} PUBLIC 
	"groupsock/include" 
	"liveMedia"
	"liveMedia/include" 
	"UsageEnvironment/include" 
	"BasicUsageEnvironment/include"
)

target_compile_definitions(${LIB_LIVE} PUBLIC BSD=1 SOCKLEN_T=socklen_t _FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE=1)

find_package(OpenSSL QUIET)
MESSAGE("OpenSSL_FOUND = ${OpenSSL_FOUND}")
if (NOT OpenSSL_FOUND)
    target_compile_definitions(${LIB_LIVE} PUBLIC NO_OPENSSL=1)
endif()


target_link_libraries(${LIB_LIVE})
if (OpenSSL_FOUND)
    target_link_libraries (${LIB_LIVE} OpenSSL::SSL OpenSSL::Crypto)
endif()
