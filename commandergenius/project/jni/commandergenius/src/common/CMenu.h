/*
 * CPassive.h
 *
 *  Created on: 24.09.2009
 *      Author: gerstrong and pizza2004
 *
 *
 * TODO: Comment about this class
 */
#ifndef CMENU_H_
#define CMENU_H_

#include <string>
#include <SDL.h>
#include "../dialog/CDialog.h"
#include "../dialog/CTextViewer.h"
#include "../fileio/CSavedGame.h"
#include "Menu/CBaseMenu.h"
#include "CMap.h"
#include "options.h"

#include "../vorticon/infoscenes/CInfoScene.h"

#define TWIRL_SPEED_SLOW        100
#define TWIRL_SPEED_FAST        20

#define MENUS_Y                 32
#define MAINMENU_X              32

#define STORYBOARD_X			672
#define STORYBOARD_Y			32

#define FONT_TWIRL              9
#define TWIRL_SPEED             30
#define TWIRL_NUM_FRAMES        5

class CMenu {

public:

	CMenu( char menu_mode, std::string &GamePath,
			char &Episode, CMap &Map,
			CSavedGame &SavedGame, stOption *pOption );
	virtual ~CMenu();

	bool init( char menu_type = MAIN );

	void process();
	void processMainMenu();
	void processQuitMenu();
	void processNumPlayersMenu();
	void processDifficultyMenu();
	void processConfigureMenu();
	void processNumControlMenu();
	void processControlMenu();
	void processF1Menu();
	void processEndGameMenu();
	void processSaveMenu();
	void processLoadMenu();
	void processOverwriteMenu();
	void processOptionsMenu();
	void processAudioMenu();

	void cleanup();

	// Getters
	char getNumPlayers() { return m_NumPlayers; }
	char getDifficulty() { return m_Difficulty; }
	bool mustEndGame() { return m_Endgame; }
	bool mustStartGame() { return (m_NumPlayers > 0 and m_Difficulty > -1); }
	bool getExitEvent() { return (m_quit); }
	bool getShowStory() { return (m_menu_type==STORY); }
	bool mustBeClosed() { return m_goback; }
	bool getBacktoDemo() { return m_demoback; }
	bool getChooseGame() { return m_choosegame; }
	bool restartVideo() { return m_RestartVideo; }
	
	void videoRestarted() { m_RestartVideo = false; }

	bool m_demoback;
	bool m_hideobjects;
	char m_mode;
	
	CDialog *mp_Dialog;

private:
	void initMainMenu();
	void initConfirmMenu();
	void initNumPlayersMenu();
	void initDifficultyMenu();
	void initConfigureMenu();
	void initSaveMenu();
	void initNumControlMenu();
	void initControlMenu();
	void initF1Menu();
	void initOptionsMenu();
	void initAudioMenu();

	CInfoScene *mp_InfoScene;
	CBaseMenu *mp_Menu;
	SDL_Surface *mp_MenuSurface;
	char &m_Episode;
	std::string &m_GamePath;
	CMap &m_Map;
	CSavedGame &m_SavedGame;
	std::vector<std::string> m_StateFileList;
	stOption *mp_option;

	bool m_choosegame;
	bool m_overwrite;
	bool m_goback;
	bool m_Endgame;
	bool m_quit;
	int m_selection;
	char m_lastselect;
	char m_menu_type;
	char m_menu_mode;
	char m_NumPlayers;
	char m_Difficulty;
	int  m_saveslot;
	bool m_RestartVideo;
};

#endif /* CMENU_H_ */
