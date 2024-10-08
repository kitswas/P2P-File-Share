#pragma once

#include <memory>

#include "../models/transaction.hpp"
#include "../network/tcp_socket.hpp"

void process_request(std::shared_ptr<Transaction> transaction, std::shared_ptr<TCPSocket> client, bool should_respond);
