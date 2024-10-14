#pragma once

#include <atomic>
#include <deque>
#include <functional>
#include <string>
#include <thread>

#include "../models/endpoint.hpp"
#include "../models/file.hpp"
#include "../models/fileinfo.hpp"
#include "../network/tcp_socket.hpp"
#include "filesdb.hpp"
#include "partfile.hpp"
#include "peerdb.hpp"

/**
 * @brief If you need a file downloaded, ask the professional.
 *
 * A DownloadManager object cannot be copied or moved.
 */
class DownloadManager
{
private:
	FilesDB &files_db;
	PeerDB &peer_db;
	const Endpoint client_endpoint;
	const EndpointID my_id;
	std::function<void(const std::string &group_id, const std::string &file_name)> on_download_complete;

	/**
	 * @brief A separate thread to download files.
	 */
	std::thread download_thread;

	/**
	 * @brief A flag to indicate if downloads should run.
	 * The download_thread observes this flag to determine if it should continue running.
	 */
	std::atomic<bool> running;

	std::deque<std::unique_ptr<PartFile>> downloads;

	/**
	 * @brief The function that the download_thread runs.
	 */
	void download_thread_function();

	void download_piece(std::unique_ptr<PartFile> &part_file, const std::string &missing_piece);

public:
	DownloadManager(FilesDB &files_db, PeerDB &peer_db, const Endpoint &client_endpoint, const EndpointID &my_id);
	~DownloadManager();
	DownloadManager(const DownloadManager &) = delete;
	DownloadManager &operator=(const DownloadManager &) = delete;
	DownloadManager(DownloadManager &&) = delete;

	bool enqueue_download(const std::string &group_id, const std::shared_ptr<FileInfo> file_info, const std::string &output_file_path);
	bool dequeue_download(const std::string &group_id, const std::string &file_name);
	bool start_downloads();
	bool pause_downloads();
	std::string list_downloads() const;
};
