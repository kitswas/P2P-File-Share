#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "file_id.hpp"
#include "partfile.hpp"

/**
 * @brief A thread-safe database of partfiles.
 *
 */
class FilesDB
{
private:
	std::mutex mutex;
	std::unordered_map<file_id, std::unique_ptr<PartFile>> files;

public:
	FilesDB() = default;
	~FilesDB() = default;
	FilesDB(const FilesDB &) = delete;
	FilesDB &operator=(const FilesDB &) = delete;
	FilesDB(FilesDB &&) = delete;

	/**
	 * @brief Add a partfile to the database.
	 *
	 * @return true If the partfile was added successfully.
	 * @return false If the partfile was not added successfully.
	 */
	bool add_partfile(const std::string &group_id, std::shared_ptr<FileInfo> file_info, const std::string &file_path, bool all_downloaded);

	/**
	 * @brief Remove a partfile from the database.
	 *
	 * @return true If the partfile was removed successfully.
	 * @return false If the partfile was not removed successfully.
	 */
	bool remove_partfile(const std::string &group_id, const std::string &file_name);

	PartFile get_partfile(const std::string &group_id, const std::string &file_name);

	/**
	 * @brief Check if a partfile exists in the database.
	 *
	 */
	bool has_file(const std::string &group_id, const std::string &file_name);

	bool has_piece(const std::string &group_id, const std::string &file_name, const std::string &piece);

	bool is_complete(const std::string &group_id, const std::string &file_name);

	bool mark_downloaded(const std::string &group_id, const std::string &file_name, const std::string &piece);
};
