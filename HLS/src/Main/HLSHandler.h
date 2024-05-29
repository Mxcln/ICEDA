#ifndef HLS_HANDLER_H
#define HLS_HANDLER_H

#include <Parser/parser.h>

namespace hls {

class HLSHandler {
public:
  HLSHandler();
  ~HLSHandler();

  void run(int argc, char *argv[]);
  void schedule();
  void binding(basic_block& bb);
private:

  function* _func;
  parser::parser* _parser; 
  
};  

} // namespace hls

#endif