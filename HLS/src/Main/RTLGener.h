#ifndef RTL_GENER_H
#define RTL_GENER_H

#include <Basic.hpp>
#include <fstream>
#include <map>
#include <string>
#include <vector>

namespace hls {

class function;

class RTLGener {
public:
  constexpr static int REG_WIDTH = 32;

  RTLGener(function *func) : func(func) {}

  void generate(const std::string &rtl_filename);

private:
  void gen_module_header(std::ofstream &out);

  void gen_module_footer(std::ofstream &out);

  void gen_module_body(std::ofstream &out);

  void signal_declaration(std::ofstream &out);

  void state_machine(std::ofstream &out);

  void analyse_function();

  void write_default_reg(std::ofstream &out);

  std::string reg_name(int reg_index);
  std::string reg_name(const std::string &var_name);

  int var_value(const std::string &var_name) {return func->get_reg_map(var_name);}

  void nonBlockAssign(std::ofstream &out, const std::string& left, const std::string& right = "0");

  void gen_port(std::ofstream &out, const std::string &port_name, bool is_input,
                bool is_array, int width, bool is_end);

  void line(std::ofstream &out, const std::string &line, int indent = 0);

  function *func;

}; // class RTLGener

} // namespace hls

#endif