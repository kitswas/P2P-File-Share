#pragma once

#include <memory>

#include "../network/tcp_socket.hpp"
#include "../models/transaction.hpp"

std::shared_ptr<Transaction> parse_request(std::shared_ptr<TCPSocket> client, const std::string &data);
