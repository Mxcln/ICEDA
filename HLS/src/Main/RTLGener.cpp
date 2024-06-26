#include <Main/RTLGener.h>

#include <cmath>
#include <fstream>
#include <iostream>

using namespace hls;

void RTLGener::generate(const std::string &rtl_filename) {
  std::ofstream out(rtl_filename);
  if (!out.is_open()) {
    std::cerr << "Error opening file: " << rtl_filename << std::endl;
    return;
  }

  analyse_function();

  gen_module_header(out);

  gen_module_body(out);

  gen_module_footer(out);

  out.close();
}

void RTLGener::gen_module_header(std::ofstream &out) {
  out << "module " << func->get_function_name() << " (\n";

  gen_port(out, "clk", true, false, 1, false);
  gen_port(out, "rst_n", true, false, 1, false);

  for (auto &var : func->get_function_params()) {
    if (var._array_flag) {
      // out << "    input wire [" << REG_WIDTH - 1 << ":0] " << var._name <<
      // ",\n"; gen_port(out, var._name, true, true, REG_WIDTH, false);

      gen_port(out, var._name + "_addr", false, true, REG_WIDTH, false);
      gen_port(out, var._name + "_data", true, true, REG_WIDTH, false);
    } else {
      // out << "    input wire " << var._name << ",\n";
      gen_port(out, var._name, true, false, REG_WIDTH, false);
    }
  }
  // out << "    input wire [" << REG_WIDTH - 1 << ":0] result\n";
  gen_port(out, "result", false, true, REG_WIDTH, true);

  out << ");" << std::endl;
}

void RTLGener::gen_module_body(std::ofstream &out) { 
    signal_declaration(out); 
    state_machine(out);    
}

void RTLGener::gen_module_footer(std::ofstream &out) {
  out << "endmodule" << std::endl;
}

void RTLGener::signal_declaration(std::ofstream &out) {
  auto reg_num = func->regNum();
  for (int i = 0; i < reg_num; i++) {
    out << "    reg [" << REG_WIDTH - 1 << ":0] reg_" << i << ";\n";
  }

  // log2 of func->blockNum();
  int state_width = std::log2(func->blockNum()) + 1;
  int counter_width = std::log2(func->maxBlockCycle()) + 1;

  out << "    reg [" << state_width - 1 << ":0] state;\n";
  out << "    reg [" << state_width - 1 << ":0] next_state;\n";
  out << "    reg [" << state_width - 1 << ":0] prev_state;\n";
  out << "    reg [" << counter_width - 1 << ":0] block_counter;\n\n";
}

void RTLGener::state_machine(std::ofstream &out) {
  out << "always @(posedge clk or negedge rst_n) begin\n";
  out << "    if (!rst_n) begin\n";
  write_default_reg(out);
  
  out << "    end\n";
  out << "    else begin\n";

  out << "        case(state)\n";
  for (int i = 0; i< func->blockNum(); i++) {
    out << "            " << i << ": begin\n";
    // inside block
    out << "                case(block_counter)\n";

    auto curr_block = func->get_basic_blocks()[i];
    auto& statement_hash = curr_block.get_statements_by_cycle();
    auto end_cycle = curr_block.get_end_cycle();

    for (auto& [count, statements] : statement_hash ) {
      out << "                    " << count << ": begin\n";
      
      for (auto statement : statements) {
        auto type = statement->get_type();
        auto left_reg = reg_name(statement->get_var());
        if (type == OP_TYPE::OP_PHI) {

            std::unordered_map<int ,int> phi_result;
            // phi_result.resize(10);
            for (int k = 0; k < statement->get_num_oprands(); k+=2) {
              auto right_reg = statement->get_oprand(k);
              auto right_reg_value = var_value(right_reg);
              auto right_block = statement->get_oprand(k+1);
              auto block_index = func->get_name_to_block(right_block);
              if (right_reg_value >= 0 ) {
                phi_result[block_index] = right_reg_value;
              } else if (right_reg_value == -1) {
                phi_result[block_index] = std::stoi(right_reg);
              }
            }

            out << "                        case(prev_state)";
            for (auto& [block_index, reg_value] : phi_result) {
                out << "                            " << block_index << ": begin\n";
                nonBlockAssign(out, left_reg, std::to_string(reg_value));
                out << "                            end\n";
            }
            out << "                        endcase\n";
          
        } else if (type == OP_TYPE::OP_BR) {

        } else if (type == OP_TYPE::OP_RET) {
          

        } else if (type == OP_TYPE::OP_LOAD) {

        } else if (type == OP_TYPE::OP_STORE) {

        } else if (type == OP_TYPE::OP_ADD) {
            auto right_reg1 = statement->get_oprand(0);
            auto right_reg2 = statement->get_oprand(1);
            // auto right_reg1_value = var_value(right_reg1);
            // auto right_reg2_value = var_value(right_reg2);
            out << "                    " << left_reg << " <= " << reg_name(right_reg1) << " + " << reg_name(right_reg2) << ";\n"; 
        } else if (type == OP_TYPE::OP_MUL) {
            auto right_reg1 = statement->get_oprand(0);
            auto right_reg2 = statement->get_oprand(1);
            out << "                    " << left_reg << " <= " << reg_name(right_reg1) << " * " << reg_name(right_reg2) << ";\n"; 
        } else if (type == OP_TYPE::OP_GT) {
            auto right_reg1 = statement->get_oprand(0);
            auto right_reg2 = statement->get_oprand(1);
            out << "                    " << left_reg << " <= " << reg_name(right_reg1) << " >= " << reg_name(right_reg2) << ";\n"; 
        } else if (type == OP_TYPE::OP_ASSIGN) {
            auto right_reg = statement->get_oprand(0);
            out << "                    " << left_reg << " <= " << reg_name(right_reg) << ";\n"; 
        }

      }

      out << "                        end\n";
    }

    out << "                endcase\n";
    out << "            end\n";
  }

  out << "            default: begin\n";
  write_default_reg(out);
  out << "            end\n";
  out << "        endcase\n";

  out << "    end\n";
  out << "end\n";
}

void RTLGener::analyse_function() {
  std::cout << "Analyse function: " << func->get_function_name() << std::endl;
}

void RTLGener::write_default_reg(std::ofstream &out) {
  nonBlockAssign(out, "state", "0");
  nonBlockAssign(out, "next_state", "0");
  nonBlockAssign(out, "prev_state", "0");
  nonBlockAssign(out, "block_counter", "0");
  for (int i = 0; i < func->regNum(); i++) {
    nonBlockAssign(out, "reg_" + std::to_string(i), "0");
  }
}

std::string RTLGener::reg_name(int reg_index) {
  return "reg_" + std::to_string(reg_index);
}

std::string RTLGener::reg_name(const std::string &var_name) {
  auto index = func->get_reg_map(var_name);
  if (index >=0) 
    return reg_name(func->get_reg_map(var_name));
  else 
    return std::to_string(index);
}

void RTLGener::nonBlockAssign(std::ofstream &out, const std::string &left,
                              const std::string &right) {
  out << "        " << left << " <= " << right << ";\n";
}

void RTLGener::gen_port(std::ofstream &out, const std::string &port_name,
                        bool is_input, bool is_array, int width, bool is_end) {
  if (is_input) {
    out << "    input wire ";
  } else {
    out << "    output wire ";
  }

  if (is_array) {
    out << "[" << width - 1 << ":0] ";
  }

  out << port_name;
  if (!is_end) {
    out << ",\n";
  } else {
    out << "\n";
  }
}

void RTLGener::line(std::ofstream &out, const std::string &line, int indent) {
  for (int i = 0; i < indent; i++) {
    out << "  ";
  }
  out << line << std::endl;
}