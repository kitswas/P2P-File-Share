#include "filesdb.hpp"

bool FilesDB::add_partfile(const std::string &group_id, std::shared_ptr<FileInfo> file_info, const std::string &file_path)
{
	std::scoped_lock lock(mutex);
	file_id key = {group_id, file_info->name};
	if (files.find(key) != files.end())
	{
		return false;
	}

	files[key] = std::make_unique<PartFile>(group_id, file_info, file_path);
	return true;
}

bool FilesDB::remove_partfile(const std::string &group_id, const std::string &file_name)
{
	std::scoped_lock lock(mutex);
	file_id key = {group_id, file_name};
	if (files.find(key) == files.end())
	{
		return false;
	}

	files.erase(key);
	return true;
}

PartFile FilesDB::get_partfile(const std::string &group_id, const std::string &file_name)
{
	std::scoped_lock lock(mutex);
	file_id key = {group_id, file_name};
	if (files.find(key) == files.end())
	{
		throw std::invalid_argument("File not found in database");
	}

	return *files[key];
}

bool FilesDB::has_file(const std::string &group_id, const std::string &file_name)
{
	std::scoped_lock lock(mutex);
	file_id key = {group_id, file_name};
	return files.find(key) != files.end();
}

bool FilesDB::has_piece(const std::string &group_id, const std::string &file_name, const std::string &piece)
{
	std::scoped_lock lock(mutex);
	file_id key = {group_id, file_name};
	if (files.find(key) == files.end())
	{
		return false;
	}

	return files[key]->has_piece(piece);
}

bool FilesDB::is_complete(const std::string &group_id, const std::string &file_name)
{
	std::scoped_lock lock(mutex);
	file_id key = {group_id, file_name};
	if (files.find(key) == files.end())
	{
		return false;
	}

	return files[key]->is_complete();
}

bool FilesDB::mark_downloaded(const std::string &group_id, const std::string &file_name, const std::string &piece)
{
	std::scoped_lock lock(mutex);
	file_id key = {group_id, file_name};
	if (files.find(key) == files.end())
	{
		return false;
	}

	return files[key]->mark_downloaded(piece);
}
