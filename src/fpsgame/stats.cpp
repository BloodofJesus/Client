#include "game.h"
#include "engine.h"
#include "cube.h"

int lasttimeupdate = 0;

namespace game
{
    int stats[27];

/**
STATS LIST

0: total shoots
1: total damage dealt
2: total jumps
3: avg accuracy
4: total deaths
6: bots killed
7: unnameds killed
8: players killed (total)
9: flags scored (ctf)
10: flags scored (protect)
11: avg. acc. factor
12: times lol'd
13: quad damages taken
14: health boosts taken
15: ammo packs taken
16: health kits taken
17: players killed with chainsaw
18: times suiced

Total play time:

19: secs
20: mins
21: hours
22: days
23: weeks

GraphOX:

25: theme changed
26: teamkills mades
27: teamkills recieved

**/

    void writestats()
    {
        stream *f = openfile("data/themes/stats", "w");
        if(!f) {
          conoutf("error creating stats file");
        } else {
	  loopi(28) {
            f->printf("stats[%d] = %d\n", i, game::stats[i]);
          }
          f->close();
	}
    }

    void readstats()
    {
      char buf[64];   //Stats might be long after longer play and i'm afraid of that 32 won't be engough in that situtation
      int statsdata[2];
      stream *f = openfile("data/themes/stats", "r");
      if(!f) {
        conoutf("error opening stats file");
      } else {
        while(f->getline(buf, sizeof(buf))) {
          sscanf(buf, "stats[%d] = %d", &statsdata[0], &statsdata[1]);
          game::stats[statsdata[0]] = statsdata[1];
        }
        f->close();
      }
    }

    void dotime()   //Time updating for 'Total play time' stats
    {
        if(totalmillis >= lasttimeupdate+1000)    //1 second interval
        {
            game::stats[19]++;  //Add one second

            if(game::stats[19] >= 60)   //After 60 secs
            {
                game::stats[19] = 0;    //Reset seconds
                game::stats[20]++;      //Add one minute
            }

            if(game::stats[20] >= 60)   //After 60 mins
            {
                game::stats[20] = 0;    //Reset mins
                game::stats[21]++;      //Add one hour
            }

            if(game::stats[21] >= 24)   //After 24 hours
            {
                game::stats[21] = 0;    //Reset hours
                game::stats[22]++;      //Add one day
            }

            if(game::stats[22] >= 7)    //After 7 days
            {
                game::stats[22] = 0;    //Reset days
                game::stats[23]++;      //Add one week
            }
            lasttimeupdate = totalmillis;
        }
    }
}

ICOMMAND(stats, "i", (int *i), intret(game::stats[*i]));
