#include <Main/HLSHandler.h>
#include <iostream>

namespace hls {

HLSHandler::HLSHandler()
{
  _func = new function();
  _parser = new parser::parser(_func);
}

HLSHandler::~HLSHandler()
{
  delete _func;
  delete _parser;
}

void HLSHandler::run(int argc, char *argv[])
{
  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
    return;
  }
  _parser->set_filepath(argv[1]);
  if (_parser->parse() != 0)
  {
    std::cerr << "Error parsing file: " << argv[1] << std::endl;
    return;
  }

  std::cout << *_func;
}

} // namespace hls