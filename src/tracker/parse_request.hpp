#pragma once

#include <memory>

#include "../network/tcp_socket.hpp"
#include "../models/transaction.hpp"

std::shared_ptr<Transaction> parse_request(const std::string &data);
