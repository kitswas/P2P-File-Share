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

/**
 * @brief Execution starts here
 */
int main(int argc, char *argv[])
{
	// Parse args
	if (argc != 3)
	{
		std::cerr << "Usage: ../client <IP>:<PORT> tracker_info.txt" << std::endl;
		exit(1);
	}
	// Ignore argv[1] for now

	// Load tracker info
	load_tracker_info(argv[2], 1);

	// Connect to tracker
	TCPSocket tracker;
	try
	{
		tracker.connect(ip, port);
	}
	catch (const std::exception &_)
	{
		load_tracker_info(argv[2], 2);
		try
		{
			tracker.connect(ip, port);
		}
		catch (const NetworkError &e)
		{
			std::cerr << e.what() << '\n';
			exit(1);
		}
	}

	// Send message to tracker
	std::string message = "Hello from client!";
	try
	{
		tracker.send_data(message);
	}
	catch (const NetworkError &e)
	{
		std::cerr << e.what() << '\n';
		exit(1);
	}

	return 0;
}
