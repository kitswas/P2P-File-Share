#pragma once

#include "../models/transaction.hpp"

std::shared_ptr<Transaction> parse_request(const std::string &data);
