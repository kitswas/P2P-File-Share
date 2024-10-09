#pragma once

#include <string>

#include "../models/endpoint.hpp"
#include "../network/tcp_socket.hpp"

bool process_input(const std::string &input, TCPSocket &tracker, EndpointID my_id, const Endpoint &client_endpoint);
