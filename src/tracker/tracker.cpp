/**
 * @file tracker.cpp
 * @brief The tracker.
 *
 * @details
 * Usage:
 *
 * ```
 * ./tracker tracker_info.txt <tracker_no>
 * ```
 *
 * tracker info file contains a list of tracker IPs and ports
 * tracker_no is the index of the tracker to use (1 or 2)
 *
 * @author Swastik Pal
 * @date 2024-10-1
 *
 * @copyright Copyright (c) 2024
 */

#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <memory>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <thread>

#include "../common/load_tracker_info.cpp"
#include "../network/network_errors.hpp"
#include "../network/tcp_server.hpp"
#include "parse_request.hpp"
#include "process_request.hpp"
#include "transactionsrecord.hpp"

static TransactionsRecord successful_transactions;

bool mirror(const Transaction &transaction, const std::vector<Endpoint> &trackers)
{
	for (const auto &tracker : trackers)
	{
		if (tracker.ip == transaction.source.ip && tracker.port == transaction.source.port)
		{
			continue;
		}
		try
		{
			auto mirror = std::make_shared<TCPSocket>();
			mirror->connect(tracker.ip, tracker.port);
			mirror->send_data(transaction.data);
		}
		catch (NetworkError &_)
		{
			std::clog << "Error mirroring data to " << tracker.ip << ":" << tracker.port << std::endl;
			return false;
		}
	}
	return true;
}

void loop(const Endpoint &endpoint, const std::vector<Endpoint> &trackers)
{
	std::function<void(std::shared_ptr<TCPSocket>)> onConnect = [](std::shared_ptr<TCPSocket> client)
	{
		std::cout << "Client connected " << client->get_peer_ip() << ":" << client->get_peer_port() << std::endl;
	};
	std::function<void(std::shared_ptr<TCPSocket>)> onDisconnect = [](std::shared_ptr<TCPSocket> client)
	{
		std::cout << "Client disconnected " << client->get_peer_ip() << ":" << client->get_peer_port() << std::endl;
	};
	std::function<void(std::shared_ptr<TCPSocket>, std::string &)> onData = [&trackers](std::shared_ptr<TCPSocket> client, std::string &data)
	{
		std::cout << "Data received from " << client->get_peer_ip() << ":" << client->get_peer_port() << " : " << data << std::endl;

		// If data is from another tracker, process it locally to ensure consistency
		bool is_tracker = false;
		for (const auto &tracker : trackers)
		{
			if (client->get_peer_ip() == tracker.ip && client->get_peer_port() == tracker.port)
			{
				is_tracker = true;
				break;
			}
		}
		try
		{
			auto transaction = parse_request(client, data);
			std::clog << "Parsed Transaction: " << transaction->to_string() << std::endl;
			process_request(transaction, client, !is_tracker);
			if (transaction->outcome.success)
			{
				successful_transactions.addTransaction(*transaction);
			}
			// if (!is_tracker && transaction->outcome.success) // Mirror the data to the other trackers
			// {
			// 	mirror(*transaction, trackers);
			// }
		}
		catch (UnknownRequest &e)
		{
			client->send_data("Unknown request\n");
			return;
		}
	};

	TCPServer server(2);
	server.setOnConnect(onConnect);
	server.setOnDisconnect(onDisconnect);
	server.setOnData(onData);
	server.start("", endpoint.port);

	while (true)
	{
		if (std::cin.peek() != EOF) // check if there is input
		{
			std::string input;
			std::cin >> input;
			std::cin.ignore(1000, '\n'); // ignore the rest of the line
			if (input == "quit")
			{
				break;
			}
		}
		// yield the thread
		std::this_thread::yield();
	}

	std::cout << "Shutting down server..." << std::endl;
	server.stop();
}

int main(int argc, char *argv[])
{
	int n = 1;
	const char *file_path = nullptr;

	// Parse args
	if (argc != 3)
	{
		std::cerr << "Usage: ./tracker tracker_info.txt <tracker_no>" << std::endl;
		exit(1);
	}
	n = std::stoi(argv[2]);
	file_path = argv[1];

	// Sanitize input
	if (n != 1 && n != 2)
	{
		std::cerr << "Invalid tracker number" << std::endl;
		exit(1);
	}

	std::vector<Endpoint> trackers = load_tracker_info(file_path);
	loop(trackers.at(n - 1), trackers);
	return 0;
}
