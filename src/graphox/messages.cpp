#include "graphox.h"

namespace GraphOX {
 
  // some basic functions and defines
  
  // to player ONLY (echo)
  void error(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    defformatstring(message)("\f3 [ ERROR ] \f4%s", msg);
    conoutfv(CON_INFO, message, args);
    va_end(args);
  }
  
  void info(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    defformatstring(message)("\f1 [ INFO ] \f4%s", msg);
    conoutfv(CON_INFO, message, args);
    va_end(args);
  }
  
  // from player to server
  void Auto(const char *msg, ...) {
	string pf = "\f0 [ AUTO ] \f4";
    va_list args;
    va_start(args, msg);
    defformatstring(message)("%s", msg);
    conoutfv(CON_CHAT, message, args);
    game::addmsg(N_TEXT, "rcs", game::player1, msg);
    va_end(args);
  }
  
}