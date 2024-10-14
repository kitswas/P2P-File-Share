#include <cstring>
#include <fcntl.h>

#include "../network/network_errors.hpp"
#include "downloadmanager.hpp"
#include "file_io.hpp"
#include "hash.hpp"

void DownloadManager::download_piece(std::unique_ptr<PartFile> &part_file, const std::string &missing_piece)
{
	// Get a list of peers that have the file
	auto peers = peer_db.getPeers(part_file->group_id, part_file->file_info->hash);

	for (auto const &peer : peers)
	{
		// Ask the peer for the piece
		std::string request = "get " + part_file->group_id + " " + part_file->file_info->name + " " + missing_piece;
		try
		{
			TCPSocket peer_socket;
			peer_socket.connect(peer->endpoint.ip, peer->endpoint.port);
			peer_socket.send_data(request);
			std::string response = peer_socket.receive_data();
			std::stringstream ss(response);
			int status;
			ss >> status;
			if (status == 1)
			{
				// Write the piece to the file
				std::string piece_data = response.substr(2);
				// Check if the piece is correct
				if (!is_piece_valid(missing_piece, piece_data))
				{
					continue;
				}
				write_piece_to_file(
					part_file->file_path,
					get_piece_index(part_file->file_info, missing_piece),
					block_size,
					piece_data);
				part_file->mark_downloaded(missing_piece);
				break;
			}
			else
			{
				// Peer does not have the piece
				continue;
			}
		}
		catch (const ConnectionClosedError &_)
		{
			// Peer is not available
			continue;
		}
	}
}

void DownloadManager::download_thread_function()
{
	while (running)
	{
		if (downloads.empty())
		{
			break; // No more downloads, don't waste CPU cycles
		}
		for (auto &part_file : downloads)
		{
			if (part_file->is_complete())
			{
				continue;
			}
			download_piece(part_file, *(part_file->remaining_pieces.begin()));
			if (part_file->is_complete())
			{
				on_download_complete(part_file->group_id, part_file->file_info->name);
			}
		}
	}
}

DownloadManager::DownloadManager(FilesDB &files_db, PeerDB &peer_db, const Endpoint &client_endpoint, const EndpointID &my_id)
	: files_db(files_db), peer_db(peer_db), client_endpoint(client_endpoint), my_id(my_id), running(false)
{
}

DownloadManager::~DownloadManager()
{
	if (running && download_thread.joinable())
	{
		running = false;
		download_thread.join();
	}
}

bool DownloadManager::enqueue_download(const std::string &group_id, const std::shared_ptr<FileInfo> file_info, const std::string &output_file_path)
{
	// Check if the file already exists on disk
	if (int fd = open(output_file_path.c_str(), O_RDONLY); fd != -1)
	{
		close(fd);
		return false;
	}

	if (downloads.empty())
	{
		downloads.emplace_back(std::make_unique<PartFile>(group_id, file_info, output_file_path, false));
		return true;
	}
	for (const auto &part_file : downloads)
	{
		if (part_file->group_id == group_id && part_file->file_info.get() == file_info.get())
		{
			return false;
		}
	}
	downloads.emplace_back(std::make_unique<PartFile>(group_id, file_info, output_file_path, false));
	files_db.add_partfile(group_id, file_info, output_file_path, false);
	return true;
}

bool DownloadManager::dequeue_download(const std::string &group_id, const std::string &file_name)
{
	std::find_if(downloads.begin(), downloads.end(),
				 [&group_id, &file_name](const std::unique_ptr<PartFile> &part_file)
				 {
					 return part_file->group_id == group_id && part_file->file_info->name == file_name;
				 });
	return false;
}

bool DownloadManager::start_downloads()
{
	if (running)
	{
		return false;
	}
	running = true;
	download_thread = std::thread(&DownloadManager::download_thread_function, this);
	return true;
}

bool DownloadManager::pause_downloads()
{
	if (!running)
	{
		return false;
	}
	running = false;
	download_thread.join();
	return true;
}

std::string DownloadManager::list_downloads() const
{
	if (downloads.empty())
	{
		return "Nothing to show\n";
	}
	std::string list = "";
	for (const auto &part_file : downloads)
	{
		std::string status = "";
		status += part_file->is_complete() ? "[C]" : "[D]";
		status += " " + part_file->group_id + " " + part_file->file_info->name;
		status += " " + std::to_string(part_file->downloaded_pieces.size()) + "/" + std::to_string(part_file->PieceCount);
		list += status + "\n";
	}
	return list;
}

void DownloadManager::set_on_download_complete(std::function<void(const std::string &group_id, const std::string &file_name)> on_download_complete)
{
	this->on_download_complete = on_download_complete;
}
