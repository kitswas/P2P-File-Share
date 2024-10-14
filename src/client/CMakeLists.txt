set(CLIENT_SOURCES
	client.cpp
	downloadmanager.cpp
	hash.cpp
	file_io.cpp
	filesdb.cpp
	partfile.cpp
	peerdb.cpp
	process_input.cpp
	process_request.cpp
)

add_executable(client ${CLIENT_SOURCES}) 
