cmake_minimum_required(VERSION 3.16)
project(P2P-File-Share)

set(CMAKE_CXX_STANDARD 17)

# Some compiler flags for GCC and Clang, disable if unwanted
if("${CMAKE_C_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_C_COMPILER_ID}" MATCHES "GNU") 
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Wconversion -Wnull-dereference -Wold-style-cast")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g -DDEBUG")
	# set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Werror") # Uncomment to treat warnings as errors in debug builds
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -s -DNDEBUG -march=native -fstack-protector-all")
endif()

add_subdirectory(src/tracker tracker)

add_subdirectory(src/client client)

add_subdirectory(src/network network)

target_link_libraries(tracker network)
target_link_libraries(client network)
