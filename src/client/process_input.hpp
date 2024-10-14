#pragma once

#include <string>

#include "../models/endpoint.hpp"
#include "../network/tcp_socket.hpp"
#include "downloadmanager.hpp"

bool process_input(const std::string &input, TCPSocket &tracker, EndpointID my_id, const Endpoint &client_endpoint, DownloadManager &download_manager, FilesDB &my_files, PeerDB &peer_db);
