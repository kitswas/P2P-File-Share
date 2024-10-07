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
#include "../network/tcp_server.hpp"
#include "process_request.hpp"

void loop(const Endpoint &endpoint)
{
	std::function<void(std::shared_ptr<TCPSocket>)> onConnect = [](std::shared_ptr<TCPSocket> client)
	{
		std::cout << "Client connected " << client->get_peer_ip() << ":" << client->get_peer_port() << std::endl;
	};
	std::function<void(std::shared_ptr<TCPSocket>)> onDisconnect = [](std::shared_ptr<TCPSocket> client)
	{
		std::cout << "Client disconnected " << client->get_peer_ip() << ":" << client->get_peer_port() << std::endl;
	};
	std::function<void(std::shared_ptr<TCPSocket>, std::string &)> onData = [](std::shared_ptr<TCPSocket> client, std::string &data)
	{
		std::cout << "Data received from " << client->get_peer_ip() << ":" << client->get_peer_port() << " : " << data << std::endl;
		process_request(client, data);
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
	loop(trackers.at(n - 1));
	return 0;
}
