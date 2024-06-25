#ifndef RTL_GENER_H
#define RTL_GENER_H


# include <string>
# include <vector>
# include <map>
# include <Basic.hpp>

namespace hls {

class function;

class RTLGener {
public:

    constexpr static int REG_WIDTH = 32;

    RTLGener(function* func) : func(func) {}
    
    void generate(const std::string& rtl_filename);

private:

    void gen_module_header(std::ofstream& out);

    void gen_module_footer(std::ofstream& out);

    void gen_module_body(std::ofstream& out);

    void signal_declaration(std::ofstream& out);

    void analyse_function();

    void gen_port(std::ofstream& out, const std::string& port_name, bool is_input, bool is_array, int width, bool is_end);

    function* func;

};  // class RTLGener

}

#endif