#ifndef BASIC_H
#define BASIC_H

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <unordered_map>

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

class reg;
class point;

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
  void set_begin_cycle(int t) { begin_cycle = t; }
  int get_begin_cycle() { return begin_cycle; }

  void add_reg(int r) { regs.push_back(r); }
  void set_regs(const std::vector<int> &r) { regs = r; }
  std::vector<int> &get_regs() { return regs; }

  int get_op_index(){return op_index;}
  void set_op_index(int index){op_index = index;}

private:
  int begin_cycle;
  int op_index;
  std::vector<int> regs;
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
  const std::string get_name() { return _label; }
  void add_statement(statement &s) { _statements.push_back(s); }
  void clear_statements() { _statements.clear(); }
  std::vector<statement> &get_statements() { return _statements; }
  std::string get_label_name() { return _label; }
  
  std::vector<std::string> &get_inputs() { return inputs; }
  std::vector<std::string> &get_outputs() { return outputs; }
  
  void add_input(const std::string &input) { inputs.push_back(input);}
  void add_output(const std::string &output) { outputs.push_back(output);}
  
  bool is_input(const std::string &input) { return std::find(inputs.begin(), inputs.end(), input) != inputs.end(); }
  bool is_output(const std::string &output) { return std::find(outputs.begin(), outputs.end(), output) != outputs.end(); }
  
  void set_end_cycle (int t) { end_cycle = t; }
  int  get_end_cycle() { return end_cycle; }

  void set_colors(int c) { colors = c; }
  int get_colors() { return colors; }

  std::vector<std::string> get_var_names(){
    std::cout << "var_names size: " << var_names.size() << std::endl;
    for(auto s : _statements){
      if(!var_names.empty()){
      auto it = std::find(var_names.begin(), var_names.end(), s.get_var());
      if(s.get_var()!="" && it == var_names.end())
        var_names.push_back(s.get_var());
      }
      else{
        var_names.push_back(s.get_var());
      }
      std::cout << "var_names size: " << var_names.size() << std::endl;
      int op_num = s.get_num_oprands();
      for(int i = 0; i < op_num; i++){
        std::cout << "oprand: " << s.get_oprand(i);
        auto it = std::find(var_names.begin(), var_names.end(), s.get_oprand(i));
        if(s.get_oprand(i)!="" && it== var_names.end())
          var_names.push_back(s.get_oprand(i));
      } 
    }
  }

  void add_statement_by_cycle(statement* s, int cycle) {
    statements_by_cycle[cycle].push_back(s);              
  }

  std::unordered_map<int,std::vector<statement*>>& get_statements_by_cycle() 
  {
    return statements_by_cycle;
  }

  std::unordered_map<std::string, point*> points;

private:
  int colors;
  int end_cycle;
  std::vector<statement> _statements;
  std::string _label;
  std::vector<std::string> inputs;
  std::vector<std::string> outputs;

  std::vector<std::string> var_names;
  std::unordered_map<int,std::vector<statement*>> statements_by_cycle;
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

  void add_global_reg(std::string reg) { global_regs.push_back(reg); }
  std::vector<std::string>& get_global_regs() {
    return global_regs;
  }

  int is_global_reg(std::string reg) {
    int i;
    for (i = 0; i < global_regs.size(); i++) {
      if(global_regs[i] == reg)
        return i;
    }
    if(i==global_regs.size())
      return -1;
  }

  void add_reg_map(std::string reg, int index) {
    reg_map[reg] = index;
  }

  int get_reg_map(std::string reg) {
    if(reg_map.find(reg) != reg_map.end())
      return reg_map[reg];
    else
      return -1;
  }

  bool is_set_reg(std::string reg) {
    if (reg_map.empty())
      return false;
    if(reg_map.find(reg) != reg_map.end())
      return true;
    else
      return false;
  }

  int get_name_to_block(std::string name) {
    for(int i = 0; i < _blocks.size(); i++) {
      if(_blocks[i].get_label_name() == name)
        return i;
    }
    return -1;
  }

private:
  std::vector<basic_block> _blocks;
  std::string _name;
  std::vector<var> _params;
  RET_TYPE _ret_type;
  std::vector<std::string> global_regs = {"c","cl","i","cr","i_inc"};    //全局寄存器 
  std::unordered_map<std::string, int> reg_map;
};

// std::ostream& operator<<(std::ostream& os, const hls::function& func);
class point {
  public:
    point(std::string &v):value(v){
      in_num = 0;
      visited = false;
    };
    ~point(){};
    int in_num;

    void set_color(int c) { color = c; }
    int get_color() { return color; }
    
    int get_begin_cycle() { return begin_cycle; }
    void set_begin_cycle(int t) { begin_cycle = t; }
    int get_end_cycle() { return end_cycle; }
    void set_end_cycle(int t) { end_cycle = t; }
    std::string get_value() { return value; }
    void set_value(const std::string &t) { value = t; }
    std::vector<std::string> &get_next_points() { return next_points; }
    void add_next_point(std::string &p) { next_points.push_back(p); }
    void add_prev_point(std::string &p) { prev_points.push_back(p); }
    std::vector<std::string> &get_prev_points() { return prev_points; }
    void set_visited() { visited = true; }
    bool is_visited() { return visited; }
    void delete_next_point(const std::string &p) { 
        auto it = std::find(next_points.begin(), next_points.end(), p);

        // 检查是否找到了该元素
        if (it != next_points.end()) {
            // 使用erase删除该元素
            next_points.erase(it);
        } else {
            std::cout << "Element " << p << " not found in the vector." << std::endl;
        }
      }

  private:
    
    int begin_cycle;
    int end_cycle;
    std::string value;
    std::vector<std::string> next_points;
    std::vector<std::string> prev_points;
    bool visited;
    int color;
};

} // namespace hls

#endif // BASIC_H