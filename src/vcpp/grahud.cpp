#include "graphox.h"

namespace GraphOX {
  int chh, cah, camh;
  float chs, cas, cams;

  void grahud(int w, int h)
  {
    fpsent *d = game::hudplayer();
    if(d->state == CS_SPECTATOR || d->state == CS_DEAD) return;
    glPushMatrix();
    glScalef(1/1.2f, 1/1.2f, 1);
    if(!insta) draw_textf("%d", 80, h*1.2f-128, d->health);
    defformatstring(ammo)("%d", d->ammo[d->gunselect]);
    int wb, hb;
    text_bounds(ammo, wb, hb);
    draw_textf("%d", w*1.2f-wb-80, h*1.2f-128, d->ammo[d->gunselect]);

    if(d->quadmillis)
    {
      settexture("data/themes/hud/hud_quaddamage_left.png");
      glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);   glVertex2f(0,   h*1.2f-207);
      glTexCoord2f(1.0f, 0.0f);   glVertex2f(539, h*1.2f-207);
      glTexCoord2f(1.0f, 1.0f);   glVertex2f(539, h*1.2f);
      glTexCoord2f(0.0f, 1.0f);   glVertex2f(0,   h*1.2f);
      glEnd();

      settexture("data/themes/hud/hud_quaddamage_right.png");
      glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);   glVertex2f(w*1.2f-135, h*1.2f-207);
      glTexCoord2f(1.0f, 0.0f);   glVertex2f(w*1.2f,     h*1.2f-207);
      glTexCoord2f(1.0f, 1.0f);   glVertex2f(w*1.2f,     h*1.2f);
      glTexCoord2f(0.0f, 1.0f);   glVertex2f(w*1.2f-135, h*1.2f);
      glEnd();
    }

    if(d->maxhealth > 100)
    {
      settexture("data/themes/hud/hud_megahealth.png");
      glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);   glVertex2f(0,   h*1.2f-207);
      glTexCoord2f(1.0f, 0.0f);   glVertex2f(539, h*1.2f-207);
      glTexCoord2f(1.0f, 1.0f);   glVertex2f(539, h*1.2f);
      glTexCoord2f(0.0f, 1.0f);   glVertex2f(0,   h*1.2f);
      glEnd();
    }

    int health = (d->health*100)/(d->maxhealth),
	armour = (d->armour*100)/200,
	hh = (health*101)/100,
	ah = (armour*167)/100;
  
    float hs = (health*1.0f)/100,
	  as = (armour*1.0f)/100;
  
    if     (chh>hh) chh -= max(1, ((chh-hh)/4));
    else if(chh<hh) chh += max(1, ((hh-chh)/4));
    if     (chs>hs) chs -= max(0.01f, ((chs-hs)/4));
    else if(chs<hs) chs += max(0.01f, ((hs-chs)/4));
  
    if     (cah>ah) cah -= max(1, ((cah-ah)/4));
    else if(cah<ah) cah += max(1, ((ah-cah)/4));
    if     (cas>as) cas -= max(0.01f, ((cas-as)/4));
    else if(cas<as) cas += max(0.01f, ((as-cas)/4));
  
    if(d->health > 0 && !insta)
    {
      settexture("data/themes/hud/hud_health.png");
      glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 1.0f-chs);   glVertex2f(47, h*1.2f-chh-56);
      glTexCoord2f(1.0f, 1.0f-chs);   glVertex2f(97, h*1.2f-chh-56);
      glTexCoord2f(1.0f, 1.0f);      glVertex2f(97, h*1.2f-57);
      glTexCoord2f(0.0f, 1.0f);      glVertex2f(47, h*1.2f-57);
      glEnd();
    }

    if(d->armour > 0)
    {
      settexture("data/themes/hud/hud_armour.png");
      glBegin(GL_QUADS);
      glTexCoord2f(0.0f,    0.0f);   glVertex2f(130,    h*1.2f-62);
      glTexCoord2f(cas,      0.0f);   glVertex2f(130+cah, h*1.2f-62);
      glTexCoord2f(cas,      1.0f);   glVertex2f(130+cah, h*1.2f-44);
      glTexCoord2f(0.0f,    1.0f);   glVertex2f(130,    h*1.2f-44);
      glEnd();
    }
  
    if(!insta)
    {
      settexture("data/themes/hud/hud_left.png");
      glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);   glVertex2f(0,   h*1.2f-207);
      glTexCoord2f(1.0f, 0.0f);   glVertex2f(539, h*1.2f-207);
      glTexCoord2f(1.0f, 1.0f);   glVertex2f(539, h*1.2f);
      glTexCoord2f(0.0f, 1.0f);   glVertex2f(0,   h*1.2f);
      glEnd();
    }

    settexture("data/themes/hud/hud_right.png");
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);   glVertex2f(w*1.2f-135, h*1.2f-207);
    glTexCoord2f(1.0f, 0.0f);   glVertex2f(w*1.2f,     h*1.2f-207);
    glTexCoord2f(1.0f, 1.0f);   glVertex2f(w*1.2f,     h*1.2f);
    glTexCoord2f(0.0f, 1.0f);   glVertex2f(w*1.2f-135, h*1.2f);
    glEnd();

    int maxammo = 0;

    switch(d->gunselect)
    {
      case GUN_FIST:
	  maxammo = 1;
	  break;
  
      case GUN_RL:
      case GUN_RIFLE:
	  maxammo = insta ? 100 : (insta ? 10 : 15);
	  break;
  
      case GUN_SG:
      case GUN_GL:
	  maxammo = insta ? 20 : 30;
	  break;
  
      case GUN_CG:
	  maxammo = insta ? 20 : 60;
	  break;
  
      case GUN_PISTOL:
	  maxammo = 120;
	  break;
    }
  
    int curammo = (d->ammo[d->gunselect]*100)/maxammo,
	amh = (curammo*101)/100;
  
    float ams = (curammo*1.0f)/100;
  
    if     (camh>amh) camh -= max(1, ((camh-amh)/4));
    else if(camh<amh) camh += max(1, ((amh-camh)/4));
    if     (cams>ams) cams -= max(0.01f, ((cams-ams)/4));
    else if(cams<ams) cams += max(0.01f, ((ams-cams)/4));

    if(d->ammo[d->gunselect] > 0)
    {
      settexture("data/themes/hud/hud_health.png");
      glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 1.0f-cams);   glVertex2f(w*1.2f-47, h*1.2f-camh-56);
      glTexCoord2f(1.0f, 1.0f-cams);   glVertex2f(w*1.2f-97, h*1.2f-camh-56);
      glTexCoord2f(1.0f, 1.0f);       glVertex2f(w*1.2f-97, h*1.2f-57);
      glTexCoord2f(0.0f, 1.0f);       glVertex2f(w*1.2f-47, h*1.2f-57);
      glEnd();
    }
  
    glPopMatrix();
    glPushMatrix();
    
    glScalef(1/4.0f, 1/4.0f, 1);
    
    defformatstring(icon)("data/themes/hud/guns/%d.png", d->gunselect);
    settexture(icon);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);   glVertex2f(w*4.0f-1162,    h*4.0f-350);
    glTexCoord2f(1.0f, 0.0f);   glVertex2f(w*4.0f-650,     h*4.0f-350);
    glTexCoord2f(1.0f, 1.0f);   glVertex2f(w*4.0f-650,     h*4.0f-50);
    glTexCoord2f(0.0f, 1.0f);   glVertex2f(w*4.0f-1162,    h*4.0f-50);
    glEnd();
  
    glPopMatrix();
    
  }
}