#pragma once

#include "../models/transaction.hpp"

void process_request(std::shared_ptr<Transaction> transaction, std::shared_ptr<TCPSocket> client, bool should_respond);
