/**
 * @file utils.h
 * @author WindaYau (windayau233@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef UTILS_H
#define UTILS_H

#include <Basic.hpp>

namespace hls::utils {

// file path validation
bool isFilePathValid(const std::string &file_path);
bool isDirectoryPathValid(const std::string &filePath);

std::string op2str(const OP_TYPE op);
std::string ret2str(const RET_TYPE ret);


void errorQuit(const std::string &error_message);

std::string exec(const char *cmd);

std::string trim(const std::string &str);

}   // namespace hls::utils

#endif  // UTILS_H