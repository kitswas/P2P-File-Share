#include <cstring>
#include <fcntl.h>
#include <memory>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>

#include "../models/file.hpp"
#include "../models/fileinfo.hpp"
#include "hash.hpp"
#include "process_input.hpp"

std::unique_ptr<FileInfo> get_file_info(const std::string &file_path)
{
	// get filename
	std::string filename = basename(file_path.c_str());
	// open file
	int fd = open(file_path.c_str(), O_RDONLY);
	if (fd == -1)
	{
		std::cerr << "Error opening file" << strerror(errno) << std::endl;
		std::cerr << "Check that filepath does not have spaces.\n"
				  << std::endl;
		return nullptr;
	}
	// get file size
	off_t file_size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET); // reset file pointer
	// read file in blocks
	std::vector<std::string> pieces;
	std::string hash = "";
	int8_t buffer[block_size];
	ssize_t bytes_read;
	while ((bytes_read = read(fd, buffer, block_size)) > 0)
	{
		// hash block, each SHA-1 hash is 20 characters long
		std::string piece = get_sha1(buffer, bytes_read);
		pieces.push_back(piece);
		hash += piece;
	}
	// close file
	close(fd);
	return std::make_unique<FileInfo>(filename, file_size, pieces, hash);
}

bool process_input(const std::string &input, TCPSocket &tracker, EndpointID my_id, const Endpoint &client_endpoint)
{
	std::string request = "1" + std::to_string(my_id) + " ";
	std::stringstream ss(input);
	std::string command;
	ss >> command;
	if (command == "upload_file")
	{
		std::string file_path;
		ss >> file_path;
		std::string group_id;
		ss >> group_id;
		auto file_info = get_file_info(file_path);
		if (!file_info)
		{
			return false;
		}
		std::cout << file_info->to_string() << std::endl;
		request += command + " " + group_id + " " + client_endpoint.to_string() + " " + file_info->to_string();
	}
	else if (command == "download_file")
	{
		std::string group_id;
		ss >> group_id;
		std::string file_name;
		ss >> file_name;
		std::string destination_path;
		ss >> destination_path;
		request += command + " " + group_id + " " + file_name;
	}
	else
	{
		request += input;
	}
	tracker.send_data(request);
	std::string response = tracker.receive_data();
	if (command == "download_file")
	{
		std::stringstream download_info(response);
		int file_found = 0;
		download_info >> file_found;
		if (download_info.fail())
		{
			std::cout << response << std::endl;
			return true;
		}
		if (file_found == 1)
		{
			auto file = File::from_string(download_info.str().substr(download_info.tellg()));
			std::cout << file.group_id << " " << file.file_info->name << std::endl;
			std::cout << file.file_info->to_string() << std::endl;
		}
		else
		{
			std::cout << download_info.str().substr(download_info.tellg()) << std::endl;
		}
	}
	else
	{
		std::cout << response << std::endl;
	}
	return true;
}
