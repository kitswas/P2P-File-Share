#pragma once

#include "downloadmanager.hpp"

bool process_input(const std::string &input, TCPSocket &tracker, EndpointID my_id, const Endpoint &client_endpoint, DownloadManager &download_manager, FilesDB &my_files, PeerDB &peer_db);
