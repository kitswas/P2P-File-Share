set(TRACKER_SOURCES
	parse_request.cpp
	process_request.cpp
	tracker.cpp
	transactionsrecord.cpp
	groupdb.cpp
	userdb.cpp
)

add_executable(tracker ${TRACKER_SOURCES}) 
