#pragma once

#include <memory>
#include <string>

#include "../models/endpoint.hpp"
#include "../network/tcp_socket.hpp"
#include "peerdb.hpp"

void process_request(std::shared_ptr<TCPSocket> client, PeerDB &peer_db, const std::string &request, const EndpointID &my_id);
