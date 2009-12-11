#include "../../sdl/sound/CSound.h"

#include "../../keen.h"
#include "../../game.h"

#include "baby.h"

// Baby Vorticon (the superfast little blue creatures that knock you down)
// (ep 2 & 3)

#define BABY_RUN        0
#define BABY_DYING      1
#define BABY_DEAD       2

#define BABY_WALK_SPEED         15

#define BABY_WALK_ANIM_RATE     20

#define BABY_WALK_LEFT_FRAME    48
#define BABY_WALK_RIGHT_FRAME   52
#define BABY_FRY_FRAME          56
#define BABY_DEAD_FRAME         57

#define BABY_JUMP_PROB			80
#define BABY_BOUNCE_PROB		10

#define BABY_FRY_TIME           80
#define BABY_DIE_INERTIA        20

#define BABY_JUMP_BIG           1
#define BABY_JUMP_SMALL         0
void baby_jump(int o, int big);
//
//void CObjectAI::baby_ai(CObject &object, int episode, bool hard)
//{
//	bool ep3;
//
//	if (object.needinit)
//	{
//		object.ai.baby.state = BABY_RUN;
//		object.ai.baby.dir = rnd()&1?LEFT:RIGHT;
//		object.ai.baby.walkframe = 0;
//		object.ai.baby.walktimer = 0;
//		object.ai.baby.inertia_x = 0;
//		object.ai.baby.inertia_y = 0;
//		object.canbezapped = 1;
//		object.needinit = 0;
//		object.blockedl = object.blockedr = object.blockedu = object.blockedd = 1;
//
//		// babies are in ep2 & ep3, but frameset starts one index prior in ep3
//		if (episode==3) ep3 = true; else ep3 = false;
//		object.sprite = BABY_WALK_RIGHT_FRAME - ep3;
//		return;
//	}
//	if (object.ai.baby.state==BABY_DEAD)
//	{
//		object.inhibitfall = 0;
//		return;
//	}
//
//	// babies are in ep2 & ep3, but frameset starts one index prior in ep3
//	if (episode==3) ep3 = true; else ep3 = false;
//
//	// jumping
//	if (object.ai.baby.inertia_y < 0 || !object.blockedd)
//	{
//		if (object.blockedu && object.ai.baby.inertia_y < 0)
//		{
//			object.ai.baby.inertia_y = 0;
//		}
//
//		object.y += object.ai.baby.inertia_y;
//
//		if (object.ai.baby.jumpdectimer >= object.ai.baby.jumpdecrate)
//		{
//			if (object.ai.baby.inertia_y < (1<<CSF))
//			{
//				object.ai.baby.inertia_y++;
//			}
//			object.ai.baby.jumpdectimer = 0;
//		}
//		else object.ai.baby.jumpdectimer++;
//	}
//	else	// blockedd = 1, and inertia_y >= 0
//	{
//		object.ai.baby.inertia_y = 0;
//		if (object.ai.baby.state == BABY_RUN)
//		{
//			if (rnd()%BABY_JUMP_PROB == (BABY_JUMP_PROB/2))
//			{
//				baby_jump(o, BABY_JUMP_SMALL);
//			}
//		}
//	}
//
//
//	// got hit?
//	if (object.zapped)
//	{
//		if (object.zapped > 1 || !hard)
//		{  // we're fried!!
//			if (object.ai.baby.state != BABY_DYING)
//			{
//				object.ai.baby.dietimer = 0;
//				object.ai.baby.state = BABY_DYING;
//				object.ai.baby.jumpdecrate = 5;
//				object.sprite = BABY_FRY_FRAME - ep3;
//				object.zapped = 0;
//				object.canbezapped = 0;
//				if (object.onscreen && !g_pSound->isPlaying(SOUND_VORT_DIE))
//					g_pSound->playStereofromCoord(SOUND_VORT_DIE, PLAY_NOW, object.scrx);
//
//				if (object.ai.baby.dir == RIGHT)
//					object.ai.baby.inertia_x = BABY_DIE_INERTIA;
//				else
//					object.ai.baby.inertia_x = -BABY_DIE_INERTIA;
//
//			}
//		}
//	}
//
//	// touched player
//	if (object.touchPlayer && object.ai.baby.state!=BABY_DYING && !player[object.touchedBy].pfrozentime)
//	{
//		player[object.touchedBy].pdir = player[object.touchedBy].pshowdir = object.ai.baby.dir;
//		freezeplayer(object.touchedBy);
//	}
//
//	switch(object.ai.baby.state)
//	{
//	case BABY_DYING:
//		if ((object.ai.baby.inertia_x < 0 && object.blockedl) ||\
//				(object.ai.baby.inertia_x > 0 && object.blockedr))
//		{
//			object.ai.baby.inertia_x = 0;
//		}
//
//		object.x += object.ai.baby.inertia_x;
//		if (object.ai.baby.xdectimer >= 10)
//		{
//			if (object.ai.baby.inertia_x < 0)
//			{
//				object.ai.baby.inertia_x++;
//			}
//			else if (object.ai.baby.inertia_x > 0)
//			{
//				object.ai.baby.inertia_x--;
//			}
//
//			if (object.ai.baby.inertia_x == 0 &&
//					object.ai.baby.inertia_y == 0 &&
//					object.sprite==(unsigned int)(BABY_DEAD_FRAME-ep3))
//			{
//				object.ai.baby.state = BABY_DEAD;
//			}
//			object.ai.baby.xdectimer = 0;
//		}
//		else object.ai.baby.xdectimer++;
//
//		if (object.ai.baby.dietimer > BABY_FRY_TIME)
//		{
//			object.sprite = BABY_DEAD_FRAME-ep3;
//		}
//		else object.ai.baby.dietimer++;
//		break;
//	case BABY_RUN:
//		// run in appropriate direction
//		if (object.ai.baby.dir==RIGHT)
//		{ // running right
//			object.sprite = BABY_WALK_RIGHT_FRAME + object.ai.baby.walkframe - ep3;
//			if (object.blockedr)
//			{
//				object.ai.baby.dir = LEFT;
//				if ((rnd()&BABY_BOUNCE_PROB)==BABY_BOUNCE_PROB/2) baby_jump(o, BABY_JUMP_BIG);
//			}
//			else
//			{
//				object.x += BABY_WALK_SPEED;
//			}
//		}
//		else
//		{ // running left
//			object.sprite = BABY_WALK_LEFT_FRAME + object.ai.baby.walkframe - ep3;
//			if (object.blockedl)
//			{
//				object.ai.baby.dir = RIGHT;
//				if ((rnd()&BABY_BOUNCE_PROB)==BABY_BOUNCE_PROB/2) baby_jump(o, BABY_JUMP_BIG);
//			}
//			else
//			{
//				object.x -= BABY_WALK_SPEED;
//				object.inhibitfall = 1;
//			}
//		}
//
//		// run animation
//		if (object.ai.baby.walktimer > BABY_WALK_ANIM_RATE)
//		{
//			object.ai.baby.walkframe++;
//			if (object.ai.baby.walkframe > 3) object.ai.baby.walkframe = 0;
//			object.ai.baby.walktimer = 0;
//		} else object.ai.baby.walktimer++;
//		break;
//	}
//}
//
//#define BABY_BIGJUMP                 25
//#define BABY_BIGJUMP_DEC_RATE        10
//
//#define BABY_MIN_SMALLJUMP             23
//#define BABY_SMALLJUMP_MIN_DEC_RATE    18
//#define BABY_MAX_SMALLJUMP             30
//#define BABY_SMALLJUMP_MAX_DEC_RATE    20
//void CObjectAI::baby_jump(int o, int big)
//{
//	if ((rnd()&2)==0) big = 1-big;
//	if (big==BABY_JUMP_BIG)
//	{
//		object.ai.baby.inertia_y = -BABY_BIGJUMP;
//		object.ai.baby.jumpdecrate = BABY_BIGJUMP_DEC_RATE;
//	}
//	else
//	{
//		object.ai.baby.inertia_y = -20;//(rnd()%(BABY_MAX_SMALLJUMP-BABY_MIN_SMALLJUMP))+BABY_MIN_SMALLJUMP;
//		object.ai.baby.jumpdecrate = 10;//(rnd()%(BABY_SMALLJUMP_MAX_DEC_RATE-BABY_SMALLJUMP_MIN_DEC_RATE))+BABY_SMALLJUMP_MIN_DEC_RATE;
//	}
//
//	object.ai.baby.jumpdectimer = 0;
//}


