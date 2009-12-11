/*
 * se.h
 *
 *  Created on: 06.08.2009
 *      Author: gerstrong
 */

#ifndef SE_H_
#define SE_H_

void se_extend_plat(int o, bool *p_PlatExtending);

/* located in game.c */
void kill_all_intersecting_tile(int mpx, int mpy);
void delete_object(int o);
void killplayer(int theplayer);
unsigned char spawn_object(int x, int y, int otype);

/* located in map.c */

//----------------------[referenced from ai/se.cpp]--------------------//
unsigned int getmaptileat(unsigned int x, unsigned int y);
void map_chgtile(unsigned int x,unsigned  int y, int newtile);
void map_deanimate(int x, int y);


/* located in misc.c */
unsigned int rnd(void);

/* located in cinematics/blowupworld.c */

void SetVibrateTime(int vibetime, char pausegame, stLevelControl *levelcontrol);



//----------------------[referenced from ai/se.c]--------------------//
void se_ai(int o, stLevelControl *p_levelcontrol);
void se_extend_plat(int o, bool *p_PlatExtending);
void se_retract_plat(int o, bool *p_PlatExtending);
void spark_ai(int o, int *p_sparks_left);
void se_ankhshield(int o, int episode);
void se_mortimer_arm(int o);
void se_mortimer_spark(int o, stLevelControl *p_levelcontrol);
void se_mortimer_heart(int o, stLevelControl *p_levelcontrol);
void se_mortimer_zapsup(int o, stLevelControl *levelcontrol);
void se_mortimer_leg_left(int o);
void se_mortimer_leg_right(int o);
void se_mortimer_randomzaps(int o);
void set_mortimer_surprised(int yes);


#endif /* SE_H_ */
