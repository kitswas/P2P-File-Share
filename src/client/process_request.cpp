#include "../models/fileinfo.hpp"
#include "file_io.hpp"
#include "process_request.hpp"

#include <fcntl.h>
#include <sstream>
#include <string>
#include <unistd.h>

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
