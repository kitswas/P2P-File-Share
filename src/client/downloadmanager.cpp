#include <cstring>
#include <fcntl.h>

#include "downloadmanager.hpp"
#include "hash.hpp"

DownloadManager::DownloadManager(PeerDB &peer_db, const Endpoint &client_endpoint, const EndpointID &my_id)
	: peer_db(peer_db), client_endpoint(client_endpoint), my_id(my_id), running(false)
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
		downloads.emplace_back(std::make_unique<PartFile>(group_id, file_info, output_file_path));
		return true;
	}
	for (const auto &part_file : downloads)
	{
		if (part_file->group_id == group_id && part_file->file_info.get() == file_info.get())
		{
			return false;
		}
	}
	downloads.emplace_back(std::make_unique<PartFile>(group_id, file_info, output_file_path));
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
