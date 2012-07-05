#include "graphox.h"

namespace GraphOX {
  ICOMMAND(_Graphox_ignore,			"s",	(char *arg, int *attempt),	ignore(game::parseplayer(arg), isconnected(*attempt > 0) ? 1 : 0));
  ICOMMAND(_Graphox_unignore,		"s",	(char *arg, int *attempt),	unignore(game::parseplayer(arg), isconnected(*attempt > 0) ? 1 : 0));
  ICOMMAND(_Graphox_isignored,		"s",	(char *arg),				intret(isignored(game::parseplayer(arg)) ? 1 : 0));
  ICOMMAND(_Graphox_getclientping,	"i",	(int *cn),					intret(getPing(*cn)));
  ICOMMAND(_Graphox_getping,		"",		(), 						intret(game::player1->ping));
  ICOMMAND(_Graphox_getclientpj,	"i",	(int *cn), 					intret(getPJ(*cn)));
  ICOMMAND(execdir,		"s",	(char *dir), 				{
																		ident *id = newident("f");
    																	if(id->type!=ID_ALIAS) return;
    																	vector<char *> files;
    																	listfiles(dir, "cfg", files);
    																	loopv(files)
    																	{
    																	  char *file = files[i];
    																	  bool redundant = false;
    																	  loopj(i) if(!strcmp(files[j], file)) { redundant = true; break; }
																		  if(redundant) { delete[] file; continue; }
																		  if(i) aliasa(id->name, file);
																		  else pushident(*id, file);
																		  defformatstring(files)("%s/%s.cfg", dir, file);
																		  execfile(files, false);
																		}
																	  });
  
  // Variables
  VAR(_Graphox_crosshairbump, 0, 0, 1);
  VAR(_Graphox_bumpcrossonkill, 0, 0, 1);
  VAR(_Graphox_bumpcrossonpickup, 0, 0, 1);
  VAR(_Graphox_bumpcrossonteleport, 0, 0, 1);
  VARP(_Graphox_hudtheme, 0, 1, 2);
  VARP(_Graphox_radartheme, 0, 1, 1);
  VARP(_Graphox_autosorry, 0, 0, 1);
  VARP(_Graphox_autonp, 0, 0, 1);
}