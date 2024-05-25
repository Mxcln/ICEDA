#ifndef BASIC_H
#define BASIC_H

#include <string>
#include <vector>

namespace hls {

enum class OP_TYPE {
  OP_ASSIGN,
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_LOAD,
  OP_STORE,
  OP_BR,
  OP_LT,
  OP_GT,
  OP_LE,
  OP_GE,
  OP_EQ,
  OP_PHI,
  OP_RET
};

enum class RET_TYPE { RET_VOID, RET_INT };

class statement {
public:
  statement(){};
  ~statement(){};

  OP_TYPE get_type() { return _type; }
  void set_type(OP_TYPE type) { _type = type; }
  int get_num_oprands() { return _num_oprands; }
  void set_num_oprands(int t) { _num_oprands = t; }
  std::string &get_oprand(int num) { return _oprands[num]; }
  void add_oprand(const std::string &oprand) { _oprands.push_back(oprand); }
  void set_var(const std::string &var) { _var = var; }
  const std::string get_var() { return _var; }

private:
  std::string _line;
  OP_TYPE _type;
  int _num_oprands;
  std::string _var;
  std::vector<std::string> _oprands;
};

struct var {
public:
  std::string _name;
  bool _array_flag; // = 0 for non-array, >= 1 for array
};

class basic_block {
public:
  basic_block() {}
  basic_block(std::string &label) { _label = label; }
  void set_name(const std::string &n) { _label = n; }
  void add_statement(statement &s) { _statements.push_back(s); }
  void clear_statements() { _statements.clear(); }
  std::vector<statement> &get_statements() { return _statements; }
  std::string get_label_name() { return _label; }

private:
  std::vector<statement> _statements;
  std::string _label;
};

class function {
public:
  function() {}
  function(const std::string &name) { _name = name; }

  void set_ret_type(const RET_TYPE t) noexcept { _ret_type = t; }
  void set_name(const std::string& name) noexcept { _name = name; }
  void add_basic_block(basic_block &bb) { _blocks.push_back(bb); }
  void add_param(const var &p) { _params.push_back(p); }

  std::vector<basic_block> &get_basic_blocks() { return _blocks; }
  std::string &get_function_name() { return _name; }
  std::vector<var> &get_function_params() { return _params; }
  RET_TYPE get_ret_type() { return _ret_type; }

  friend std::ostream& operator<<(std::ostream& os, function& func);

private:
  std::vector<basic_block> _blocks;
  std::string _name;
  std::vector<var> _params;
  RET_TYPE _ret_type;
};

// std::ostream& operator<<(std::ostream& os, const hls::function& func);

} // namespace hls

#endif // BASIC_H