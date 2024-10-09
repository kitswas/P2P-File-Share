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

#include "../common/generate_id.cpp"
#include "../common/load_tracker_info.cpp"
#include "../network/network_errors.hpp"
#include "../network/tcp_server.hpp"
#include "parse_request.hpp"
#include "process_request.hpp"
#include "transactionsrecord.hpp"

static EndpointID my_id;
TransactionsRecord successful_transactions;

void mirror(const Transaction &transaction, const std::vector<Endpoint> &trackers)
{
	for (const auto &tracker : trackers)
	{
		if (generate_id(tracker) == my_id)
		{
			continue;
		}
		try
		{
			TCPSocket mirror;
			mirror.connect(tracker.ip, tracker.port);
			std::string request = "0" + transaction.data;
			mirror.send_data(request);
			mirror.disconnect();
		}
		catch (ConnectionClosedError &_)
		{
			std::clog << "Tracker at " << tracker.ip << ":" << tracker.port << " is down" << std::endl;
		}
		catch (NetworkError &e)
		{
			std::cerr << "Error mirroring data to " << tracker.ip << ":" << tracker.port << " - " << e.what() << std::endl;
		}
	}
}

void process_data(std::shared_ptr<TCPSocket> client, std::string &data, const std::vector<Endpoint> &trackers)
{
	try
	{
		std::shared_ptr<Transaction> transaction;
		bool do_mirror = data[0] == '1'; // 1 for mirror
		data = data.substr(1);			 // Strip the mirror flag
		transaction = parse_request(data);
		std::clog << "Parsed Transaction: ";
		std::clog << transaction->to_string() << std::endl;
		process_request(transaction, client, do_mirror);
		if (transaction->outcome.success)
		{
			successful_transactions.addTransaction(*transaction);
			if (do_mirror)
				mirror(*transaction, trackers); // Mirror the data to the other trackers
		}
	}
	catch (UnknownRequest &e)
	{
		client->send_data(e.what());
		return;
	}
}

/**
 * @brief Attempt to connect to any of the trackers, get the list of successful transactions,
 * and execute them on this tracker to bring it up to date.
 */
bool sync_with_online_trackers(const std::vector<Endpoint> &trackers)
{
	TCPSocket tracker;
	for (auto const &t : trackers)
	{
		if (generate_id(t) == my_id)
		{
			continue;
		}
		try
		{
			if (tracker.connect(t.ip, t.port))
			{
				std::clog << "Connected to tracker " << t.ip << ":" << t.port << std::endl;
				std::string sync_request = "0" + std::to_string(my_id) + " sync \n";
				tracker.send_data(sync_request);
				while (true)
				{
					std::string data = tracker.receive_data();
					std::clog << "Received data: " << data << std::endl;
					if (data.empty() || data == "done")
					{
						break;
					}
					tracker.send_data("next"); // Acknowledge the data
					std::shared_ptr<Transaction> transaction = parse_request(data);
					process_request(transaction, nullptr, false);
				}
				tracker.disconnect();
				std::clog << "Synced with tracker " << t.ip << ":" << t.port << std::endl;

				return true;
			}
		}
		catch (const ConnectionClosedError &e)
		{
			std::cerr << t.ip << ":" << t.port << " " << e.what() << std::endl;
		}
	}
	return false;
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
		process_data(client, data, trackers);
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
	my_id = generate_id(trackers.at(n - 1));
	std::cout << "My ID: " << my_id << std::endl;
	sync_with_online_trackers(trackers);
	try
	{
		loop(trackers.at(n - 1), trackers);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
