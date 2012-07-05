#include "graphox.h"

namespace GraphOX {
    struct playerinfo {
        string name, team;
        int cn,ping,frags,flags,deaths,teamkills,accuracy,privilege,state;
        uint ip;
        playerinfo() : cn(-1),ping(0),frags(0),flags(0),deaths(0),teamkills(0),accuracy(0),privilege(0),state(0),ip(0) {}
    };
    struct teaminfo {
        string name;
        int score;
        int numbases;
        vector<int> bases; //deemed useless information in relation to a server browser, however it shall be stored for future use TODO: is this really needed?
        vector<playerinfo *> players;
        teaminfo() : score(0), numbases(-1) {}
    };
    struct extinfo {
        int gametime;
        int teamping,playerping,playerids;
        bool teamresp,playerresp;
        int gamemode;
        bool teammode, ignore;
        vector<teaminfo *> teams;
        vector<playerinfo *> players,spectators;
        extinfo() : gametime(0),teamping(0),playerping(0),playerids(0),teamresp(true),playerresp(true),gamemode(-3),teammode(false), ignore(false) {}
    };

    static int sortplayers(playerinfo ** a, playerinfo ** b) {
        if((*a)->flags > (*b)->flags) return -1;
        if((*a)->flags < (*b)->flags) return 1;
        if((*a)->frags > (*b)->frags) return -1;
        if((*a)->frags < (*b)->frags) return 1;
        return strcmp((*a)->name, (*b)->name);
        
    }
    static int sortteams(teaminfo ** a, teaminfo ** b)
    {
        if(!(*a)->name[0])
        {
            if((*b)->name[0]) return 1;
        }
        else if(!(*b)->name[0]) return -1;
        if((*a)->score > (*b)->score) return -1;
        if((*a)->score < (*b)->score) return 1;
        if((*a)->players.length() > (*b)->players.length()) return -1;
        if((*a)->players.length() < (*b)->players.length()) return 1;
        return (*a)->name[0] && (*b)->name[0] ? strcmp((*a)->name, (*b)->name) : 0;
    }
    void assignplayers(serverinfo * si)
    {
        extinfo * x = (extinfo *)si->extinfo;
        if(x->teammode) {
            loopv(x->teams) x->teams[i]->players.shrink(0);
            loopv(x->players) {
                playerinfo * pl = x->players[i];
                loopv(x->teams) if(!strncmp(pl->team,x->teams[i]->name,260) && pl->state != CS_SPECTATOR) x->teams[i]->players.add(pl);
            }
        }
        else
        {
            teaminfo * tm = NULL;
            if(x->teams.length() == 0) {
                tm= new teaminfo;
                x->teams.add(tm);
                strcpy(tm->name, "");
                tm->score = 0;
            }
            else tm = x->teams[0];
            tm->players = x->players;
            loopv(tm->players) if(tm->players[i]->state == CS_SPECTATOR) tm->players.remove(i);
        }
        x->spectators.shrink(0);
        loopv(x->players) {
            if(x->players[i]->state == CS_SPECTATOR) x->spectators.add(x->players[i]);
        }
        loopv(x->teams) x->teams[i]->players.sort(sortplayers);
        x->teams.sort(sortteams);
    }

    bool extinfoparse(ucharbuf & p, serverinfo * si) {
            if(getint(p) != 0) return false;
            playerinfo * pl = NULL;
            teaminfo * tm = NULL;
            extinfo * x = NULL;
            if(si->extinfo == NULL) {
                x = new extinfo;
                si->extinfo = x;
            } else x = (extinfo *)si->extinfo;

            int op = getint(p);
            if(op == 1) { //playerinfo
                //Failsafe sanity check to prevent overflows
                if(x->ignore) return true;
                if(x->players.length() > MAXCLIENTS || x->teams.length() > MAXCLIENTS) {
                    x->ignore = true;
                    loopv(x->players) {
                        pl = x->players[i];
                        x->players.remove(i);
                        delete pl;
                    }
                    loopv(x->teams) {
                        tm = x->teams[i];
                        x->teams.remove(i);
                        delete tm;
                    }
                }



                char text[MAXTRANS];
                uint ip = 0;
                p.len = 6;
                op = getint(p);
                if(op == -10) {
                    //check id list agains currenyly know list of players and remove missing
                    x->playerping = lastmillis;
                    vector <int> cnlist;
                    loopi(si->numplayers) {
                        int cn = getint(p);
                        cnlist.add(cn);
                    }
                    loopv(x->players) if(cnlist.find(x->players[i]->cn) == -1) {
                        delete x->players[i];
                        x->players.remove(i);
                    }
                    x->playerids = cnlist.length();
                }
                else if(op == -11) {
                    x->playerping = lastmillis;
                    int cn = getint(p);
                    //get old, or create new player
                    loopv(x->players) if(x->players[i]->cn == cn) pl = x->players[i];
                    if(!pl) {
                        pl = new playerinfo;
                        pl->cn = cn;
                        x->players.add(pl);
                    }

                    pl->ping = getint(p);
                    getstring(text,p);//name
                    strncpy(pl->name,text,260);
                    getstring(text,p);//team
                    strncpy(pl->team,text,260);
                    pl->frags = getint(p);
                    pl->flags = getint(p);
                    pl->deaths = getint(p);
                    pl->teamkills = getint(p);
                    pl->accuracy = getint(p);
                    p.len +=3;
                    pl->privilege = getint(p);
                    pl->state = getint(p);
                    p.get((uchar*)&ip,3); //ip
                    pl->ip = ip;
                    if(x->playerids == x->players.length()) x->playerresp = true;
                }
            }
            else if(op == 2) { //teaminfo
                x->teamping = lastmillis;
                x->teamresp = true;
                char text[MAXTRANS];
                vector<teaminfo *> tmlist;
                vector<int> bases;
                p.len = 5;
                int teammode = getint(p);
                x->teammode = teammode != 0 ? false : true;
                x->gamemode = getint(p);
                x->gametime = getint(p);

                if(teammode != 0) {
                    return true; //no teams
                }
                while(p.remaining()) {
                    tm = NULL;
                    getstring(text,p);
                    int score = getint(p);
                    int numbases = getint(p);
                    if(numbases > -1) {
                        if(numbases > MAXCLIENTS) return true; //sanity check
                        loopi(numbases) bases.add(getint(p));
                    }
                    
                    loopv(x->teams) if(!strncmp(x->teams[i]->name,text,260)) {
                        tm = x->teams[i];
                    }
                    if(!tm) {
                        tm = new teaminfo();
                        strncpy(tm->name,text,260);
                        x->teams.add(tm);
                    }
                    tmlist.add(tm);
                    tm->score = score;
                    tm->numbases = numbases;
                    tm->bases = bases;
                }
                //delete missing teams
                loopv(x->teams) if(tmlist.find(x->teams[i])==-1) {
                    delete x->teams[i];
                    x->teams.remove(i);
                }
            }
            return true;
    }

    void extinforequest(ENetSocket &pingsock, ENetAddress & address,serverinfo * si) {
        extinfo * x = NULL;
        if(si != NULL)x = (extinfo *)si->extinfo;
        if(x != NULL && x->ignore) return;
        ENetBuffer playerbuf,teambuf;
        uchar player[MAXTRANS], team[MAXTRANS];
        ucharbuf pl(player, sizeof(player));
        ucharbuf tm(team, sizeof(team));
        putint(pl, 0);
        putint(pl, 1);
        putint(pl, -1);
        putint(tm, 0);
        putint(tm, 2);
        putint(tm, -1);
        if(x == NULL || (lastmillis - x->playerping > 1000 && x->playerresp) || lastmillis - x->playerping > 10000) {//only needed every second and wait for response before requesting again in case of errors, re-request every 10 seconds.
            if(x != NULL) {
                x->playerping = 0;
                x->playerresp = false;
            }
            playerbuf.data = player;
            playerbuf.dataLength = pl.length();
            enet_socket_send(pingsock, &address, &playerbuf, 1);
        }

        if(x == NULL || (lastmillis - x->teamping > 2000 && x->teamresp)) {//only needed every two second and wait for response before requesting again
            if(x != NULL) {
                x->teamping = 0;
                x->teamresp = false;
            }
            teambuf.data = team;
            teambuf.dataLength = tm.length();
            enet_socket_send(pingsock, &address, &teambuf, 1);
        }
    }

    VARP(_Graphox_extinfoshow,0,1,1);
    VARP(_Graphox_extinfoshowfrags,0,1,1);
    VARP(_Graphox_extinfoshowdeaths,0,1,1);
    VARP(_Graphox_extinfoshowteamkills,0,1,1);
    VARP(_Graphox_extinfoshowaccuracy,0,1,1);
    VARP(_Graphox_extinfoshowflags,0,1,1);
    VARP(_Graphox_extinfoshowping,0,0,1);
    VARP(_Graphox_extinfoshowclientnum,0,0,1);

    void serverextinfo(g3d_gui *cgui, serverinfo * si) {
        cgui->titlef("%.25s", 0xFFFF80, NULL, si->sdesc);
        cgui->titlef("%s:%d", 0xFFFF80, NULL, si->name, si->port);
        if(_Graphox_extinfoshow && si->extinfo != NULL) {
            extinfo * x = (extinfo *)si->extinfo;
            assignplayers(si);
            cgui->pushlist(0);
            cgui->text(server::modename(x->gamemode), 0xFFFF80);
            cgui->separator();
            const char *mname = si->map;
            cgui->text(mname[0] ? mname : "[new map]", 0xFFFF80);
            cgui->separator();
            if(x->gametime == 0 || !x->players.length()) cgui->text("intermission", 0xFFFF80);
            else
            {
                int subsecs = (lastmillis-x->teamping)/1000, secs = x->gametime-subsecs, mins = secs/60;
                secs %= 60;
                cgui->pushlist();
                cgui->strut(mins >= 10 ? 4.5f : 3.5f);
                cgui->textf("%d:%02d", 0xFFFF80, NULL, mins, secs);
                cgui->poplist();
            }
            cgui->poplist();

            cgui->separator();
 
            loopk(x->teammode ? x->teams.length() : 1)
            {
                if((k%2)==0) cgui->pushlist();
            
                teaminfo * tm = x->teams[k];
                int bgcolor = 0x3030C0, fgcolor = 0xFFFF80;

                cgui->pushlist();
                cgui->pushlist();

                #define loopscoregroup(o, b) \
                    loopv(tm->players) \
                    { \
                        playerinfo * o = tm->players[i]; \
                        b; \
                    }    

                cgui->pushlist();
                if(tm->name[0] && x->teammode)
                {
                    cgui->pushlist();
                    cgui->background(bgcolor, x->teams.length()>1 ? 3 : 5);
                    cgui->strut(1);
                    cgui->poplist();
                }
                cgui->text("", 0, " ");
                loopscoregroup(o,
                {
                    if(o->flags > 0) { cgui->pushlist(); cgui->background(tm->name[0] && x->teammode ? 0x505050 : 0, x->teams.length()>1 ? 3 : 5); }
                    const char *icon = o->state == CS_ALIVE ? "radio_on" : "radio_off";
                    cgui->text("", 0, icon);
                    if(o->flags > 0) cgui->poplist();
                });
                cgui->poplist();

                if(tm->name[0] && x->teammode)
                {
                    cgui->pushlist();

                    if(tm->score>=10000) cgui->textf("%s: WIN", fgcolor, NULL, tm->name);
                    else cgui->textf("%s: %d", fgcolor, NULL, tm->name, tm->score);

                    cgui->pushlist();
                }

                if(_Graphox_extinfoshowfrags)
                { 
                    cgui->pushlist();
                    cgui->strut(7);
                    cgui->text("frags", fgcolor);
                    loopscoregroup(o, cgui->textf("%d", 0xFFFFDD, NULL, o->frags));
                    cgui->poplist();
                }

                if(_Graphox_extinfoshowdeaths)
                { 
                    cgui->pushlist();
                    cgui->strut(7);
                    cgui->text("deaths", fgcolor);
                    loopscoregroup(o, cgui->textf("%d", 0xFFFFDD, NULL, o->deaths));
                    cgui->poplist();
                }

                if(_Graphox_extinfoshowteamkills)
                { 
                    cgui->pushlist();
                    cgui->strut(7);
                    cgui->text("teamkills", fgcolor);
                    loopscoregroup(o, cgui->textf("%d", o->teamkills >= 5 ? 0xFF0000 : 0xFFFFDD, NULL, o->teamkills));
                    cgui->poplist();
                }

                if(_Graphox_extinfoshowaccuracy)
                { 
                    cgui->pushlist();
                    cgui->strut(7);
                    cgui->text("%", fgcolor);
                    loopscoregroup(o, cgui->textf("%d%%", 0xFFFFDD, NULL, o->accuracy));
                    cgui->poplist();
                }

                if(_Graphox_extinfoshowflags && (m_check(x->gamemode,M_CTF) || m_check(x->gamemode,M_HOLD)))
                { 
                    cgui->pushlist();
                    cgui->strut(7);
                    cgui->text("flags", fgcolor);
                    loopscoregroup(o, cgui->textf("%d", 0xFFFFDD, NULL, o->flags));
                    cgui->poplist();
                }

                if(_Graphox_extinfoshowping)
                {
                    cgui->pushlist();
                    cgui->text("ping", fgcolor);
                    cgui->strut(6);
                    loopscoregroup(o, 
                    {
                        if(o->state==CS_LAGGED) cgui->text("LAG", 0xFFFFDD);
                        else cgui->textf("%d", 0xFFFFDD, NULL, o->ping);
                    });
                    cgui->poplist();
                }

                cgui->pushlist();
                cgui->text("name", fgcolor);
                cgui->strut(10);
                loopscoregroup(o, 
                {
                    int status = o->state!=CS_DEAD ? 0xFFFFDD : 0x606060;
                    if(o->privilege)
                    {
                        status = o->privilege>=2 ? 0xFF8000 : 0x40FF80;
                        if(o->state==CS_DEAD) status = (status>>1)&0x7F7F7F;
                    }
                    if(o->cn < MAXCLIENTS) cgui->text(o->name, status);
                    else cgui->textf("%s \f5[%i]", status, NULL,o->name,o->cn);
                });
                cgui->poplist();

                if(_Graphox_extinfoshowclientnum)
                {
                    cgui->space(1);
                    cgui->pushlist();
                    cgui->text("cn", fgcolor);
                    loopscoregroup(o, cgui->textf("%d", 0xFFFFDD, NULL, o->cn));
                    cgui->poplist();
                }
            
                if(tm->name[0] && x->teammode)
                {
                    cgui->poplist();
                    cgui->poplist();
                }

                cgui->poplist();
                cgui->poplist();

                if(k+1<x->teams.length() && (k+1)%2) cgui->space(2);
                else cgui->poplist();
            }
        
            if(x->spectators.length() > 0)
            {

                cgui->pushlist();

                cgui->pushlist();
                cgui->text("spectator", 0xFFFF80, " ");
                loopv(x->spectators) 
                {
                    playerinfo *pl = x->spectators[i];
                    int status = 0xFFFFDD;
                    if(pl->privilege) status = pl->privilege>=2 ? 0xFF8000 : 0x40FF80;
                    cgui->text(pl->name, status, "spectator");
                }
                cgui->poplist();

                if(_Graphox_extinfoshowclientnum) {
                    cgui->space(1);
                    cgui->pushlist();
                    cgui->text("cn", 0xFFFF80);
                    loopv(x->spectators) cgui->textf("%d", 0xFFFFDD, NULL, x->spectators[i]->cn);
                    cgui->poplist();
                }

                if(_Graphox_extinfoshowping) {
                    cgui->space(1);
                    cgui->pushlist();
                    cgui->text("ping", 0xFFFF80);
                    loopv(x->spectators) cgui->textf("%d", 0xFFFFDD, NULL, x->spectators[i]->ping);
                    cgui->poplist();
                }

                cgui->poplist();
            }


        }
    }

    bool matchstr(char * haystack, const char * straw) {
        char * copy_haystack = new char[strlen(haystack)];
        char * copy_straw = new char[strlen(straw)];
        char * r = NULL;
        for(unsigned int it = 0; it < strlen(haystack); it++) {
            copy_haystack[it] = tolower(haystack[it]);
        }

        for(unsigned int it = 0; it < strlen(straw); it++) {
            copy_straw[it] = tolower(straw[it]);
        }
        r = strstr(copy_haystack,copy_straw);
        return r && r != NULL;

    }

    SVAR(_Graphox_serverfilter, "");
    VAR(_Graphox_serverfilterplayernames, 0, 1, 1);
    VAR(_Graphox_serverfilterservernames, 0, 1, 1);
	VAR(_Graphox_serverfiltercase, 0, 1, 1);

    int serverfilter(serverinfo * si) {
        extinfo * x = (extinfo *)si->extinfo;
        if(!_Graphox_serverfilter[0]) return true;
        if(x == NULL) return false;
        int is = 0;

        if(_Graphox_serverfilterplayernames == 1) {
            loopv(x->players) {
                if(!_Graphox_serverfiltercase) {
                    if(matchstr(x->players[i]->name, _Graphox_serverfilter)) is++;
                }
                else {
                    if(strstr(x->players[i]->name, _Graphox_serverfilter)) is++;
                }
            }
        }

        if(_Graphox_serverfilterservernames == 1) {
            if(!_Graphox_serverfiltercase) {
                if(matchstr(si->sdesc, _Graphox_serverfilter)) is++;
            }
            else {
                if(strstr(si->sdesc, _Graphox_serverfilter)) is++;
            }
        }
        return is;
    }
}