/**
 * @file client.cpp
 * @brief The entrypoint to the client.
 * @author Swastik Pal
 * @date 2024-10-1
 *
 * @copyright Copyright (c) 2024
 */

#include <cstring>
#include <fcntl.h>
#include <iostream>

#include "../network/network_errors.hpp"
#include "../network/tcp_socket.hpp"

struct Tracker
{
	std::string ip;
	uint16_t port;
};

// List of trackers
static std::vector<Tracker> trackers;

void load_tracker_info(const char *file)
{
	int fd = open(file, O_RDONLY);
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
	while (true)
	{
		char ip[len];
		uint16_t port;
		if (sscanf(line, "%s %hu", ip, &port) != 2)
		{
			break;
		}
		line = strchr(line, '\n') + 1;
		trackers.push_back({ip, port});
		std::cout << "Tracker " << i << " IP: " << ip << " Port: " << port << std::endl;
		++i;
	}
}

/**
 * @brief Attempt to connect to any of the trackers
 */
bool connect_to_tracker(TCPSocket &tracker)
{
	for (auto const &t : trackers)
	{
		try
		{
			if (tracker.connect(t.ip, t.port))
			{
				std::clog << "Connected to tracker " << t.ip << ":" << t.port << std::endl;
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

/**
 * @brief Execution starts here
 */
int main(int argc, char *argv[])
{
	const char *file_path = nullptr;

	// Parse args
	if (argc != 3)
	{
		std::cerr << "Usage: ../client <IP>:<PORT> tracker_info.txt" << std::endl;
		exit(1);
	}
	// Ignore argv[1] for now

	file_path = argv[2];

	// Load tracker info
	load_tracker_info(file_path);

	// Connect to tracker
	TCPSocket tracker;
	if (!connect_to_tracker(tracker))
	{
		std::cerr << "Failed to connect to trackers." << std::endl;
		exit(1);
	}

	// Parse user input
	while (true)
	{
		std::string input;
		std::string response;
		if (std::cin.peek() != EOF) // check if there is input
		{
			std::getline(std::cin >> std::ws, input);
			if (input == "quit")
			{
				break;
			}
		}
		try
		{
			tracker.send_data(input);
			response = tracker.receive_data();
			std::cout << response << std::endl;
		}
		catch (const ConnectionClosedError &e)
		{
			std::cerr << e.what() << '\n';
			std::cerr << "Failed to send or receive data. Reconnecting..." << std::endl;
			tracker = TCPSocket(); // Create a new socket
			if (!connect_to_tracker(tracker))
			{
				std::cerr << "Failed to reconnect" << std::endl;
				exit(1);
			}
		}
	}

	return 0;
}
