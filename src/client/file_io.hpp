#pragma once

#include <memory>

#include "../models/file.hpp"

std::shared_ptr<FileInfo> get_file_info(const std::string &file_path);

size_t get_piece_index(const std::shared_ptr<FileInfo> &file_info, std::string_view piece);

bool is_piece_valid(std::string_view hash, std::string_view data);

std::string read_piece_from_file(const std::string &file_path, size_t block_index, size_t block_size);

ssize_t write_piece_to_file(const std::string &file_path, size_t block_index, size_t block_size, const std::string &data);
