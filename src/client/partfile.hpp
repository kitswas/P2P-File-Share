#pragma once

#include <memory>
#include <string>
#include <unordered_set>

#include "../models/fileinfo.hpp"

class PartFile
{
public:
	const std::string group_id;
	const std::shared_ptr<FileInfo> file_info;
	const size_t PieceCount;
	const std::string output_file_path;
	std::unordered_set<std::string> downloaded_pieces;
	std::unordered_set<std::string> remaining_pieces;

	explicit PartFile(const std::string &group_id, std::shared_ptr<FileInfo> file_info, const std::string &output_file_path)
		: group_id(group_id), file_info(file_info), PieceCount(file_info->pieces.size()), output_file_path(output_file_path)
	{
		for (const auto &piece : file_info->pieces)
		{
			remaining_pieces.insert(piece);
		}
	}

	PartFile() = delete;
	PartFile(const PartFile &src) = default;
	PartFile &operator=(const PartFile &src) = default;
	PartFile(PartFile &&src) = default;

	bool is_complete() const
	{
		return remaining_pieces.empty();
	}

	bool has_piece(const std::string &piece) const
	{
		return downloaded_pieces.find(piece) != downloaded_pieces.end();
	}

	bool mark_downloaded(const std::string &piece)
	{
		if (downloaded_pieces.find(piece) != downloaded_pieces.end())
		{
			return false;
		}
		downloaded_pieces.insert(piece);
		remaining_pieces.erase(piece);
		return true;
	}

	friend bool operator==(const PartFile &lhs, const PartFile &rhs)
	{
		return lhs.group_id == rhs.group_id && *(lhs.file_info) == *(rhs.file_info);
	}
};
