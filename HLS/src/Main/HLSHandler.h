#ifndef HLS_HANDLER_H
#define HLS_HANDLER_H

#include "Main/RTLGener.h"
#include <Parser/parser.h>

namespace hls {
class RTLGener;
class HLSHandler {
public:
  HLSHandler();
  ~HLSHandler();

  void run(int argc, char *argv[]);
  void schedule();  
  void global_binding();
  void binding(basic_block& bb,int& color);
private:

  function* _func;
  parser::parser* _parser; 
  RTLGener* _rtl_gener;

};  

} // namespace hls

#endif