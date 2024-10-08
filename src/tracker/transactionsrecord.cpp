#include "transactionsrecord.hpp"

void TransactionsRecord::addTransaction(const Transaction &transaction)
{
	transactions.push_back(transaction);
}

std::vector<Transaction> TransactionsRecord::getTransactions() const
{
	return transactions;
}
