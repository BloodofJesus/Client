// the interface the engine uses to run the gameplay module

namespace entities
{
    extern void editent(int i, bool local);
    extern const char *entnameinfo(entity &e);
    extern const char *entname(int i);
    extern int extraentinfosize();
    extern void writeent(entity &e, char *buf);
    extern void readent(entity &e, char *buf);
    extern float dropheight(entity &e);
    extern void fixentity(extentity &e);
    extern void entradius(extentity &e, bool color);
    extern bool mayattach(extentity &e);
    extern bool attachent(extentity &e, extentity &a);
    extern bool printent(extentity &e, char *buf);
    extern extentity *newentity();
    extern void deleteentity(extentity *e);
    extern void clearents();
    extern vector<extentity *> &getents();
    extern const char *entmodel(const entity &e);
    extern void animatemapmodel(const extentity &e, int &anim, int &basetime);
}

namespace game
{
    extern void parseoptions(vector<const char *> &args);

    extern void gamedisconnect(bool cleanup);
    extern void parsepacketclient(int chan, packetbuf &p);
    extern void connectattempt(const char *name, const char *password, const ENetAddress &address);
    extern void connectfail();
    extern void gameconnect(bool _remote);
    extern bool allowedittoggle();
    extern void edittoggled(bool on);
    extern void writeclientinfo(stream *f);
    extern void toserver(char *text);
    extern void changemap(const char *name);
    extern void forceedit(const char *name);
    extern bool ispaused();

    extern const char *gameident();
    extern const char *savedconfig();
    extern const char *restoreconfig();
    extern const char *defaultconfig();
    extern const char *autoexec();
    extern const char *savedservers();
    extern void loadconfigs();

    extern void updateworld();
    extern void initclient();
    extern void physicstrigger(physent *d, bool local, int floorlevel, int waterlevel, int material = 0);
    extern void bounced(physent *d, const vec &surface);
    extern void edittrigger(const selinfo &sel, int op, int arg1 = 0, int arg2 = 0, int arg3 = 0);
    extern void vartrigger(ident *id);
    extern void dynentcollide(physent *d, physent *o, const vec &dir);
    extern const char *getclientmap();
    extern const char *getmapinfo();
    extern void resetgamestate();
    extern void suicide(physent *d);
    extern void newmap(int size);
    extern void startmap(const char *name);
    extern void preload();
    extern float abovegameplayhud(int w, int h);
    extern void gameplayhud(int w, int h);
    extern bool canjump();
    extern bool allowmove(physent *d);
    extern void doattack(bool on);
    extern dynent *iterdynents(int i);
    extern int numdynents();
    extern void rendergame(bool mainpass);
    extern void renderavatar();
    extern void writegamedata(vector<char> &extras);
    extern void readgamedata(vector<char> &extras);
    extern int clipconsole(int w, int h);
    extern void g3d_gamemenus();
    extern const char *defaultcrosshair(int index);
    extern int selectcrosshair(float &r, float &g, float &b, int &w, int &h);
    extern void lighteffects(dynent *d, vec &color, vec &dir);
    extern void setupcamera();
    extern bool detachcamera();
    extern bool collidecamera();
    extern void adddynlights();
    extern void particletrack(physent *owner, vec &o, vec &d);
    extern void dynlighttrack(physent *owner, vec &o);
    extern bool serverinfostartcolumn(g3d_gui *g, int i);
    extern void serverinfoendcolumn(g3d_gui *g, int i);
    extern bool serverinfoentry(g3d_gui *g, int i, const char *name, int port, const char *desc, const char *map, int ping, const vector<int> &attr, int np);
    extern bool needminimap();

    // ANYTHING BELOW THIS RELATED TO GRAPHOX
    extern int stats[27]; // Q009, SauerEnhanced
    extern void statsacc(); // Q009, SauerEnhanced
    extern void writestats(); // Q009, SauerEnhanced
    extern void dotime(); // Q009, SauerEnhanced
    extern void readstats(); // Q009, SauerEnhanced
    extern void readstats();
}

namespace server
{
    extern void *newclientinfo();
    extern void deleteclientinfo(void *ci);
    extern void serverinit();
    extern int reserveclients();
    extern int numchannels();
    extern void clientdisconnect(int n);
    extern int clientconnect(int n, uint ip);
    extern void localdisconnect(int n);
    extern void localconnect(int n);
    extern bool allowbroadcast(int n);
    extern void recordpacket(int chan, void *data, int len);
    extern void parsepacket(int sender, int chan, packetbuf &p);
    extern void sendservmsg(const char *s);
    extern bool sendpackets(bool force = false);
    extern void serverinforeply(ucharbuf &req, ucharbuf &p);
    extern void serverupdate();
    extern bool servercompatible(char *name, char *sdec, char *map, int ping, const vector<int> &attr, int np);
    extern int laninfoport();
    extern int serverinfoport(int servport = -1);
    extern int serverport(int infoport = -1);
    extern const char *defaultmaster();
    extern int masterport();
    extern void processmasterinput(const char *cmd, int cmdlen, const char *args);
    extern bool ispaused();
	
	extern const char *modename(int, const char *unknown = "unknown");
	extern const char *mastermodename(int, const char *unknown = "unknown");
}

struct serverinfo
{
    string name, map, sdesc;
    int port, numplayers, ping, resolved, lastping;
    int mastermode;
	int mode;
	vector<int> attr;
    ENetAddress address;
    bool keep;
    const char *password;
	void * extinfo;

    serverinfo()
     : port(-1), numplayers(0), ping(INT_MAX), resolved(0), lastping(-1), keep(false), password(NULL), extinfo(NULL)
    {
        name[0] = map[0] = sdesc[0] = '\0';
    }

    ~serverinfo()
    {
        DELETEA(password);
        delete extinfo;
    }

    void reset()
    {
        lastping = -1;
    }

    void checkdecay(int decay)
    {
        if(lastping >= 0 && totalmillis - lastping >= decay)
        {
            ping = INT_MAX;
            numplayers = 0;
            lastping = -1;
        }
        if(lastping < 0) lastping = totalmillis;
    }

    void addping(int rtt, int millis)
    {
        if(millis >= lastping) lastping = -1;
        if(ping == INT_MAX) ping = rtt;
        else ping = (ping*4 + rtt)/5;
    };

    static int compare(serverinfo **ap, serverinfo **bp) //a_teammate 26.02.2011
    {
        serverinfo *a = *ap, *b = *bp;
        bool ac = server::servercompatible(a->name, a->sdesc, a->map, a->ping, a->attr, a->numplayers),
             bc = server::servercompatible(b->name, b->sdesc, b->map, b->ping, b->attr, b->numplayers);
        if(ac > bc) return -1;
        if(bc > ac) return 1;

        if(a->keep > b->keep) return -1;
        if(a->keep < b->keep) return 1;

#ifndef _RPGGAME_
		if(sortbythat == 1){ /*ping*/
				if(a->ping > b->ping) return 1;
				if(a->ping < b->ping) return -1;
		}
		if(sortbythat == 2){ /*players*/
				if(a->numplayers < b->numplayers) return 1;
				if(a->numplayers > b->numplayers) return -1;
		}
		if(sortbythat == 3){ /*map*/
				int cmpall = strcmp(a->map, b->map);
				if(cmpall != 0) return cmpall;
		}
		if(sortbythat == 4){ /*mode*/
			int cmp = strcmp(a->attr.length()>=5 ? server::modename(a->attr[1]) : "", b->attr.length()>=5 ? server::modename(b->attr[1]) : "");
			if(cmp != 0) return cmp;
		}
		if(sortbythat == 5){ /*master*/
			int cmp = strcmp(a->attr.length()>=5 ? server::mastermodename(a->attr[4]) : "",b->attr.length()>=5 ? server::mastermodename(b->attr[4]) : "");
			if(cmp != 0) return cmp;
		}
		if(sortbythat == 6){ /*host*/
		        int cmp = strcmp(a->name, b->name);
				if(cmp != 0) return cmp;
		}
		if(sortbythat == 7){ /*port*/
				if(a->port < b->port) return -1;
				if(a->port > b->port) return 1;
		}
		if(sortbythat == 8){ /*description*/
		        int cmp = strcmp(a->sdesc, b->sdesc);
				if(cmp != 0) return cmp;
		}
#endif
        if(a->numplayers < b->numplayers) return 1;
        if(a->numplayers > b->numplayers) return -1;
        if(a->ping > b->ping) return 1;
        if(a->ping < b->ping) return -1;
        int cmp = strcmp(a->name, b->name);
        if(cmp != 0) return cmp;
        if(a->port < b->port) return -1;
        if(a->port > b->port) return 1;
        return 0;
    }
};

namespace GraphOX {
    extern void sehud(int w, int h);
    extern void grahud(int w, int);

    extern int getPing(int cn);
    extern int getPJ(int cn);
    extern bool isignored(int cn);
    extern void ignore(int cn, int attempt);
    extern void unignore(int cn, int attempt);
    extern vector<int> ignores;

    // variables
    extern int _Graphox_hudtheme;
    extern int _Graphox_radartheme;
    extern int _Graphox_crosshairbump;
    extern int _Graphox_bumpcrossonkill;
    extern int _Graphox_bumpcrossonpickup;
    extern int _Graphox_bumpcrossonteleport;
    extern int _Graphox_autosorry;
    extern int _Graphox_autonp;

    // messages below here (add in struct?)
    extern void error(const char *msg, ...);
    extern void Auto(const char *msg, ...);
    extern void info(const char *msg, ...);

	// advanced server browser
	extern void serverextinfo(g3d_gui *cgui, serverinfo * si);
    extern int serverfilter(serverinfo * si);
    extern void extinforequest(ENetSocket &pingsock, ENetAddress & address,serverinfo * si);
    extern bool extinfoparse(ucharbuf & p, serverinfo * si);
}