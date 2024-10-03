#pragma once

#include <memory>
#include <string>

#include "../network/tcp_socket.hpp"

void process_request(std::shared_ptr<TCPSocket> client, std::string &data);
