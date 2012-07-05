#include "graphox.h"

namespace GraphOX {  
  int getPing(int cn) {
    fpsent *d = game::getclient(cn);
    return d ? d->ping : -1;
  }

  int getPJ(int cn) {
    fpsent *d = game::getclient(cn);
    return d ? d->ping : -1;
  }

  vector<int> ignores;
  bool isignored(int cn) {return ignores.find(cn) >= 0;}

  void ignore(int cn, int attempt) {
    fpsent *d = game::getclient(cn);
    if(attempt == 0) {error("please connect to a server to ignore");}
    else if(!d) {error("please enter a valid client number");}
    else if(d == game::player1) {error("you cannot ignore yourself");}
    else if(isignored(cn)) {error("user %s is already being ignored", d->name);}
    else {
      info("ignoring %s", d->name);
      if(ignores.find(cn) < 0) ignores.add(cn);
    }
  }

  void unignore(int cn, int attempt) {
    fpsent *d = game::getclient(cn);
    if(attempt == 0) {error("please connect to a server to ignore");}
    else if(!d) {error("please enter a valid client number");}
    else if(d == game::player1) {error("you cannot ignore yourself");}
    else if(!isignored(cn)) {error("user %s is not ignored", d->name);}
    else {
      info("stopped ignoring %s", d->name);
      ignores.removeobj(cn);
    }
  }
}