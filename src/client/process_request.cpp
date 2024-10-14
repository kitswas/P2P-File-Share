#include "../models/fileinfo.hpp"
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

size_t get_piece_index(const std::shared_ptr<FileInfo> &file_info, std::string_view piece)
{
	size_t piece_index = -1;
	for (size_t i = 0; i < file_info->pieces.size(); i++)
	{
		if (file_info->pieces[i] == piece)
		{
			piece_index = i;
			break;
		}
	}

	return piece_index;
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
			// Get the file info
			PartFile part_file = my_files.get_partfile(group_id, file_name);

			// Get the piece index
			size_t piece_index = get_piece_index(part_file.file_info, piece);

			// Get the piece from the file
			std::string piece_data = read_piece_from_file(part_file.file_path, piece_index, block_size);

			// Send the piece to the client
			client->send_data("1 " + piece_data);
		}
	}
	else
	{
		client->send_data("0 Invalid command");
	}
}
