/**
 * @file load_tracker_info.cpp
 * @brief This file should be included directly in the source file.
 * This is to prevent having to create yet another library.
 */

#pragma once

#include "../models/endpoint.hpp"

#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <vector>

std::vector<Endpoint> load_tracker_info(const char *file)
{
	std::vector<Endpoint> trackers;
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
	return trackers;
}
