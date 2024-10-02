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
#include <sys/stat.h>
#include <unistd.h>
#include <thread>

#include "../network/tcp_server.hpp"

static std::string ip;
static uint16_t port = 0;

void load_tracker_info(const char *file, int n)
{
	int fd = open(file, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1)
	{
		std::cerr << "Error opening file" << strerror(errno) << std::endl;
		exit(1);
	}
	constexpr size_t len = 1024;
	char contents[len];
	ssize_t valread = read(fd, contents, len); // read the file in one shot, assuming it's small
	close(fd);
	if (valread == -1)
	{
		std::cerr << "Error reading file" << strerror(errno) << std::endl;
		exit(1);
	}
	int i = 1;
	char *line = contents;
	while (i <= n)
	{
		char buffer[len];
		sscanf(line, "%s %hu", buffer, &port);
		line = strchr(line, '\n') + 1;
		ip = buffer;
		++i;
	}
	std::cout << "IP: " << ip << " Port: " << port << std::endl;
}

void loop()
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
	};

	TCPServer server(2);
	server.setOnConnect(onConnect);
	server.setOnDisconnect(onDisconnect);
	server.setOnData(onData);
	server.start(port);

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
	// Parse args
	if (argc != 3)
	{
		std::cerr << "Usage: ./tracker tracker_info.txt <tracker_no>" << std::endl;
		exit(1);
	}
	int n = std::stoi(argv[2]);
	// Sanitize input
	if (n != 1 && n != 2)
	{
		std::cerr << "Invalid tracker number" << std::endl;
		exit(1);
	}

	load_tracker_info(argv[1], n);
	loop();
	return 0;
}
