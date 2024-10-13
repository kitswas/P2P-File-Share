#include "process_request.hpp"

#include <fcntl.h>
#include <sstream>
#include <string>
#include <unistd.h>

std::string read_piece_from_file(const std::string &file_path, size_t block_index, size_t block_size)
{
	int fd = open(file_path.c_str(), O_RDONLY);
	if (fd == -1)
	{
		throw std::runtime_error("Error opening file");
	}

	// Seek to the start of the piece
	lseek(fd, block_index * block_size, SEEK_SET);

	// Read the piece
	char buffer[block_size];
	ssize_t bytes_read = read(fd, buffer, block_size);
	close(fd);

	if (bytes_read == -1)
	{
		throw std::runtime_error("Error reading file");
	}

	return std::string(buffer, bytes_read);
}

void process_request(std::shared_ptr<TCPSocket> client, PeerDB &peer_db, FilesDB &my_files, const std::string &request, const EndpointID &my_id)
{
	Endpoint client_endpoint = {client->get_peer_ip(), client->get_peer_port()};
	std::stringstream ss(request);
	std::string command;
	ss >> command;

	if (command == "get")
	{
		std::string group_id;
		ss >> group_id;
		std::string file_name;
		ss >> file_name;
		std::string piece;
		ss >> piece;

		// Check if the piece exists
		if (!my_files.has_piece(group_id, file_name, piece))
		{
			client->send_data("0 Piece not found");
			return;
		}
		else
		{

		}
	}
	else
	{
		client->send_data("0 Invalid command");
	}
}
