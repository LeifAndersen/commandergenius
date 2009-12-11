/*
 * CCredits.h
 *
 *  Created on: 20.06.2009
 *      Author: gerstrong
 */

#ifndef CCREDITS_H_
#define CCREDITS_H_

#include <vector>
#include <string>
#include "CInfoScene.h"
#include "../../common/CMap.h"

class CCredits : public CInfoScene {
public:
	CCredits(std::string &datadirectory, char &episode);
	virtual ~CCredits();
	
	void process();
	
private:
	char m_scrolltext[54][80];
	int m_timer;
	int m_scrolly;
	
	CMap *mp_Map;
	SDL_Surface *mp_Scrollsurface;
};

#endif /* CCREDITS_H_ */
