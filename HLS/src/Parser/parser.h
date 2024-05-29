#ifndef PARSER_H_
#define PARSER_H_

#include <Basic.hpp>
#include <fstream>

namespace hls::parser {

class parser {
public:
  parser(function* func) : _func(func) {}
  parser(const std::string &filename);
  void set_filepath(const std::string& path) {_filename = path;}
  int parse();
  std::string &get_current_line() { return _line; }

private:
  int parse_function(std::string &line);
  int parse_statement(std::string &line, basic_block &bb);
  function *get_current_function() { return _func; }

private:
  std::string _filename;
  function* _func;
  std::string _line;
};


} // namespace hls::parser

#endif
