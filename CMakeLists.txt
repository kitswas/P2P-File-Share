cmake_minimum_required(VERSION 3.16)
project(P2P-File-Share)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Some compiler flags for GCC and Clang, disable if unwanted
if("${CMAKE_C_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_C_COMPILER_ID}" MATCHES "GNU") 
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Wconversion -Wnull-dereference -Wold-style-cast")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g -DDEBUG")
	# set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Werror") # Uncomment to treat warnings as errors in debug builds
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -s -DNDEBUG -march=native -fstack-protector-all")
endif()

set(OPENSSL_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/openssl-src)
set(OPENSSL_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/openssl)
set(OPENSSL_INCLUDE_DIR ${OPENSSL_INSTALL_DIR}/include)
set(OPENSSL_LIBRARY_SUFFIX "a")
file(MAKE_DIRECTORY ${OPENSSL_INCLUDE_DIR})
file(MAKE_DIRECTORY ${OPENSSL_INSTALL_DIR}/lib64/)

include(ExternalProject)
ExternalProject_Add(
	OpenSSL
	SOURCE_DIR                 ${OPENSSL_SOURCE_DIR}
	URL                        https://github.com/openssl/openssl/releases/download/openssl-3.3.2/openssl-3.3.2.tar.gz
	URL_HASH                   SHA256=2e8a40b01979afe8be0bbfb3de5dc1c6709fedb46d6c89c10da114ab5fc3d281
	DOWNLOAD_EXTRACT_TIMESTAMP ON
	CONFIGURE_COMMAND          ${OPENSSL_SOURCE_DIR}/Configure
	                           --prefix=${OPENSSL_INSTALL_DIR}
	                           --openssldir=${OPENSSL_INSTALL_DIR}
	BUILD_COMMAND              make -j8
	INSTALL_COMMAND            make install
	INSTALL_DIR                ${OPENSSL_INSTALL_DIR}
)

link_directories(${OPENSSL_INSTALL_DIR}/lib64/)

add_library(OpenSSL::SSL STATIC IMPORTED GLOBAL)
set_property(TARGET OpenSSL::SSL PROPERTY IMPORTED_LOCATION ${OPENSSL_INSTALL_DIR}/lib64/libssl.${OPENSSL_LIBRARY_SUFFIX})
set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR})
add_dependencies(OpenSSL::SSL OpenSSL)
target_include_directories(OpenSSL::SSL INTERFACE ${OPENSSL_INCLUDE_DIR})

add_library(OpenSSL::Crypto STATIC IMPORTED GLOBAL)
set_property(TARGET OpenSSL::Crypto PROPERTY IMPORTED_LOCATION ${OPENSSL_INSTALL_DIR}/lib64/libcrypto.${OPENSSL_LIBRARY_SUFFIX})
set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR})
add_dependencies(OpenSSL::Crypto OpenSSL)
target_include_directories(OpenSSL::Crypto INTERFACE ${OPENSSL_INCLUDE_DIR})

add_subdirectory(src/tracker tracker)

add_subdirectory(src/client client)

add_subdirectory(src/network network)

target_link_libraries(tracker network)
target_link_libraries(client network OpenSSL::SSL OpenSSL::Crypto)
