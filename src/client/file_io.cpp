#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <unistd.h>

#include "hash.hpp"
#include "file_io.hpp"

std::shared_ptr<FileInfo> get_file_info(const std::string &file_path)
{
	// get filename
	std::string filename = basename(file_path.c_str());
	// open file
	int fd = open(file_path.c_str(), O_RDONLY);
	if (fd == -1)
	{
		std::cerr << "Error opening file" << strerror(errno) << std::endl;
		std::cerr << "Check that filepath does not have spaces.\n"
				  << std::endl;
		return nullptr;
	}
	// get file size
	off_t file_size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET); // reset file pointer
	// read file in blocks
	std::vector<std::string> pieces;
	std::string hash = "";
	int8_t buffer[block_size];
	ssize_t bytes_read;
	while ((bytes_read = read(fd, buffer, block_size)) > 0)
	{
		// hash block, each SHA-1 hash is 20 characters long
		std::string piece = get_sha1(buffer, bytes_read);
		pieces.push_back(piece);
		hash += piece;
	}
	// close file
	close(fd);
	return std::make_shared<FileInfo>(filename, file_size, pieces, hash);
}

size_t get_piece_index(const std::shared_ptr<FileInfo> &file_info, std::string_view piece)
{
	size_t piece_index = -1;
	for (size_t i = 0; i < file_info->pieces.size(); i++)
	{
		if (file_info->pieces[i] == piece)
		{
			piece_index = i;
			break;
		}
	}

	return piece_index;
}

bool is_piece_valid(std::string_view hash, std::string_view data)
{
	return hash == get_sha1(reinterpret_cast<const int8_t *>(data.data()), data.size());
}

std::string read_piece_from_file(const std::string &file_path, size_t block_index, size_t block_size)
{
	int fd = open(file_path.c_str(), O_RDONLY);
	if (fd == -1)
	{
		throw std::runtime_error("Error opening file");
	}

	// Seek to the start of the piece
	lseek(fd, block_index * block_size, SEEK_SET);

	// Read the piece
	char buffer[block_size];
	ssize_t bytes_read = read(fd, buffer, block_size);
	close(fd);

	if (bytes_read == -1)
	{
		throw std::runtime_error("Error reading file");
	}

	return std::string(buffer, bytes_read);
}

ssize_t write_piece_to_file(const std::string &file_path, size_t block_index, size_t block_size, const std::string &data)
{
	int fd = open(file_path.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd == -1)
	{
		throw std::runtime_error("Error opening file");
	}

	// Seek to the start of the piece
	lseek(fd, block_index * block_size, SEEK_SET);

	// Write the piece
	ssize_t bytes_written = write(fd, data.c_str(), data.size());
	close(fd);

	if (bytes_written == -1)
	{
		throw std::runtime_error("Error writing to file");
	}

	return bytes_written;
}
