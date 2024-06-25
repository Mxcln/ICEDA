#include <Main/RTLGener.h>

# include <iostream>
# include <fstream>

using namespace hls;

void RTLGener::generate(const std::string& rtl_filename)
{
    std::ofstream out(rtl_filename);
    if (!out.is_open())
    {
        std::cerr << "Error opening file: " << rtl_filename << std::endl;
        return;
    }

    analyse_function();

    gen_module_header(out);

    gen_module_body(out);

    gen_module_footer(out);

    out.close();
    
}

void RTLGener::gen_module_header(std::ofstream& out)
{
    out << "module " << func->get_function_name() << " (\n";

    gen_port(out, "clk", true, false, 1, false);
    gen_port(out, "rst", true, false, 1, false);

    for (auto& var : func->get_function_params() )
    {
        if (var._array_flag)
        {
            // out << "    input wire [" << REG_WIDTH - 1 << ":0] " << var._name << ",\n";
            // gen_port(out, var._name, true, true, REG_WIDTH, false);

            gen_port(out, var._name + "_addr", false, true, REG_WIDTH, false);
            gen_port(out, var._name + "_data", true, true, REG_WIDTH, false);
        }
        else
        {
            // out << "    input wire " << var._name << ",\n";
            gen_port(out, var._name, true, false, REG_WIDTH, false);
        }
    }
    // out << "    input wire [" << REG_WIDTH - 1 << ":0] result\n";
    gen_port(out, "result", false, true, REG_WIDTH, true);

    out << ");" << std::endl;

}

void RTLGener::gen_module_body(std::ofstream& out)
{
    signal_declaration(out);
}

void RTLGener::gen_module_footer(std::ofstream& out)
{
    out << "endmodule" << std::endl;
}

void RTLGener::signal_declaration(std::ofstream& out)
{
    auto reg_num = func->regNum();
    for (int i = 0; i < reg_num; i++) {
        out << "    reg [" << REG_WIDTH - 1 << ":0] reg_" << i << ";\n";
    }

    auto state_width = 

}

void RTLGener::analyse_function()
{
    std::cout << "Analyse function: " << func->get_function_name() << std::endl;
}

void RTLGener::gen_port(std::ofstream& out, const std::string& port_name, bool is_input, bool is_array, int width, bool is_end)
{
    if (is_input)
    {
        out << "    input wire ";
    }
    else
    {
        out << "    output wire ";
    }

    if (is_array)
    {
        out << "[" << width - 1 << ":0] ";
    }

    out << port_name;
    if (!is_end)
    {
        out << ",\n";
    }
    else
    {
        out << "\n";
    }

}