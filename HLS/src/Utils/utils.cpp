/**
 * @file utils.cpp
 * @author WindaYau (windayau233@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-17
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <Utils/Log.h>
#include <Utils/utils.h>
#include <filesystem>
#include <iostream>
#include <stdlib.h>
#include <array> 

namespace hls::utils {

bool isFilePathValid(const std::string &file_path) {
  std::filesystem::path file(file_path);
  return std::filesystem::exists(file) &&
         std::filesystem::is_regular_file(file);
}

bool isDirectoryPathValid(const std::string &filePath) {
  std::filesystem::path directoryPath =
      std::filesystem::path(filePath).parent_path();
  return std::filesystem::exists(directoryPath) &&
         std::filesystem::is_directory(directoryPath);
}

std::string op2str(const OP_TYPE op) {
  switch (op) {
  case OP_TYPE::OP_ASSIGN:
    return "OP_ASSIGN";
  case OP_TYPE::OP_ADD:
    return "OP_ADD";
  case OP_TYPE::OP_SUB:
    return "OP_SUB";
  case OP_TYPE::OP_MUL:
    return "OP_MUL";
  case OP_TYPE::OP_DIV:
    return "OP_DIV";
  case OP_TYPE::OP_LOAD:
    return "OP_LOAD";
  case OP_TYPE::OP_STORE:
    return "OP_STORE";
  case OP_TYPE::OP_BR:
    return "OP_BR";
  case OP_TYPE::OP_LT:
    return "OP_LT";
  case OP_TYPE::OP_GT:
    return "OP_GT";
  case OP_TYPE::OP_LE:
    return "OP_LE";
  case OP_TYPE::OP_GE:
    return "OP_GE";
  case OP_TYPE::OP_EQ:
    return "OP_EQ";
  case OP_TYPE::OP_PHI:
    return "OP_PHI";
  case OP_TYPE::OP_RET:
    return "OP_RET";
  default:
    return "OP_UNKNOWN";
  }
}

std::string ret2str(const RET_TYPE ret) {
  switch (ret) {
  case RET_TYPE::RET_VOID:
    return "RET_VOID";
  case RET_TYPE::RET_INT:
    return "RET_INT";
  default:
    return "RET_UNKNOWN";
  }
}

void errorQuit(const std::string &error_message) {
  std::cerr << error_message << std::endl;
  LOG_ERROR(error_message);
  exit(EXIT_FAILURE);
}

std::string exec(const char *cmd) {
  std::array<char, 128> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return result;
}

// 去除字符串两端的空格
std::string trim(const std::string &s) {
  auto start = s.begin();
  while (start != s.end() && std::isspace(static_cast<unsigned char>(*start))) {
    start++;
  }

  auto end = s.end();
  do {
    end--;
  } while (std::distance(start, end) > 0 &&
           std::isspace(static_cast<unsigned char>(*end)));

  return std::string(start, end + 1);
}

} // namespace hls::utils

namespace hls {

std::ostream &operator<<(std::ostream &os, function &func) {
  std::vector<basic_block> bbs = func.get_basic_blocks();
  std::string fn_name = func.get_function_name();
  std::vector<var> vars = func.get_function_params();
  auto ret_type = func.get_ret_type();

  if (ret_type == hls::RET_TYPE::RET_INT)
    os << "ret type: int" << std::endl;
  else
    os << "ret type: void" << std::endl;

  os << "function name" << fn_name << std::endl;

  for (int i = 0; i < vars.size(); ++i) {
    if (vars[i]._array_flag)
      os << "array" << std::endl;
    else
      os << "non-array" << std::endl;
    os << vars[i]._name << std::endl;
  }

  for (int i = 0; i < bbs.size(); ++i) {
    os << "Basic Block label: " << bbs[i].get_label_name() << std::endl;
    std::vector<statement> ss = bbs[i].get_statements();
    for (int j = 0; j < ss.size(); ++j) {
      // auto type = ss[j].get_type();
      // if((type != OP_TYPE::OP_STORE) || (type != OP_TYPE::OP_RET))
      // {
      os << "value " << ss[j].get_var() << " " << std::endl;
      // }

      os << "OP TYPE: " << utils::op2str(ss[j].get_type()) << std::endl;
      for (int k = 0; k < ss[j].get_num_oprands(); ++k)
        os << ss[j].get_oprand(k) << " ";
      os << std::endl;
    }
  }
  return os;
}

} // namespace hls
