void gamedo_RenderScreen();

void gamedo_HandleFKeys(stCloneKeenPlus *pCKP);
void gamedo_frameskipping_blitonly();
void gamedo_getInput(stLevelControl *p_levelcontrol);

// gamedo.c
int gamedo_ScrollTriggers(int theplayer);
void gamedo_AnimatedTiles();

void gamedo_render_drawobjects();

// Enemies AI Functions used in gamepdo.cpp
void baby_ai(int o, int episode, bool hard);
