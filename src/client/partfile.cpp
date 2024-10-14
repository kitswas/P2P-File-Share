#include <cstring>
#include <fcntl.h>
#include <unistd.h>

#include "partfile.hpp"

PartFile::PartFile(const std::string &group_id, std::shared_ptr<FileInfo> file_info, const std::string &file_path, bool all_downloaded)
	: group_id(group_id), file_info(file_info), PieceCount(file_info->pieces.size()), file_path(file_path)
{
	if (all_downloaded)
	{
		for (const auto &piece : file_info->pieces)
		{
			downloaded_pieces.insert(piece);
		}
	}
	else
	{
		for (const auto &piece : file_info->pieces)
		{
			remaining_pieces.insert(piece);
		}
	}
}

bool PartFile::is_complete() const
{
	return remaining_pieces.empty();
}

bool PartFile::has_piece(const std::string &piece) const
{
	return downloaded_pieces.find(piece) != downloaded_pieces.end();
}

bool PartFile::mark_downloaded(const std::string &piece)
{
	if (downloaded_pieces.find(piece) != downloaded_pieces.end())
	{
		return false;
	}
	downloaded_pieces.insert(piece);
	remaining_pieces.erase(piece);
	return true;
}
