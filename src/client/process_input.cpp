#include <cstring>
#include <fcntl.h>
#include <memory>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>

#include "../models/fileinfo.hpp"
#include "file_io.hpp"
#include "process_input.hpp"

bool process_input(const std::string &input, TCPSocket &tracker, EndpointID my_id, const Endpoint &client_endpoint, DownloadManager &download_manager, FilesDB &my_files, PeerDB &peer_db)
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
		my_files.add_partfile(group_id, file_info, file_path, true);
		tracker.send_data(request);
		std::string response = tracker.receive_data();
		std::cout << response << std::endl;
	}
	else if (command == "download_file")
	{
		std::string group_id;
		ss >> group_id;
		std::string file_name;
		ss >> file_name;
		std::string destination_path;
		ss >> destination_path;
		if (destination_path.empty())
		{
			return false; // Destination path is required
		}
		request += command + " " + group_id + " " + file_name;
		tracker.send_data(request);
		std::string response = tracker.receive_data();
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
			File file = File::from_string(download_info.str().substr(download_info.tellg()));
			// Add seeders to the peer database
			for (const auto &seeder : file.seeders)
			{
				peer_db.addPeer(Endpoint::generate_id(*seeder), *seeder);
				peer_db.addFile(Endpoint::generate_id(*seeder), file.group_id, file.file_info->hash);
			}
			std::cout << std::boolalpha << download_manager.enqueue_download(file.group_id, file.file_info, destination_path) << std::endl;
			download_manager.start_downloads();
		}
		else
		{
			std::cout << download_info.str().substr(download_info.tellg()) << std::endl;
		}
	}
	else if (command == "show_downloads")
	{
		std::cout << download_manager.list_downloads() << std::endl;
		return true;
	}
	else
	{
		request += input;
		tracker.send_data(request);
		std::string response = tracker.receive_data();
		std::cout << response << std::endl;
	}
	return true;
}
