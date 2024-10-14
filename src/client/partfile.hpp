#pragma once

#include <memory>
#include <unordered_set>

#include "../models/fileinfo.hpp"

class PartFile
{
public:
	const std::string group_id;
	const std::shared_ptr<FileInfo> file_info;
	const size_t PieceCount;
	const std::string file_path;
	std::unordered_set<std::string> downloaded_pieces;
	std::unordered_set<std::string> remaining_pieces;

	explicit PartFile(const std::string &group_id, std::shared_ptr<FileInfo> file_info, const std::string &file_path, bool all_downloaded);

	PartFile() = delete;
	PartFile(const PartFile &src) = default;
	PartFile &operator=(const PartFile &src) = default;
	PartFile(PartFile &&src) = default;

	bool is_complete() const;

	bool has_piece(const std::string &piece) const;

	bool mark_downloaded(const std::string &piece);

	friend bool operator==(const PartFile &lhs, const PartFile &rhs)
	{
		return lhs.group_id == rhs.group_id && *(lhs.file_info) == *(rhs.file_info);
	}
};

// Enable hashing for PartFile
namespace std
{
	template <>
	struct hash<PartFile>
	{
		std::size_t operator()(const PartFile &part_file) const
		{
			return std::hash<std::string>()(part_file.group_id) ^ std::hash<std::string>()(part_file.file_info->hash);
		}
	};
	
	template <>
	struct hash<PartFile*>
	{
		std::size_t operator()(const PartFile* part_file) const
		{
			return std::hash<std::string>()(part_file->group_id) ^ std::hash<std::string>()(part_file->file_info->hash);
		}
	};
}
