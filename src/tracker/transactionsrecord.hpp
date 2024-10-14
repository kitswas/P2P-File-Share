#pragma once

#include "../models/transaction.hpp"

/**
 * @brief Once a Transaction is added to the Record, it is immutable.
 *
 */
class TransactionsRecord
{
private:
	std::vector<Transaction> transactions;

public:
	TransactionsRecord() = default;

	void addTransaction(const Transaction &transaction);

	std::vector<Transaction> getTransactions() const;
};
