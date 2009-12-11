/*
 * CSettings.h
 *
 *  Created on: 08.06.2009
 *      Author: gerstrong and albert
 */

#ifndef CSETTINGS_H_
#define CSETTINGS_H_

#include <string>
#include "../common/options.h"

#define CONFIGFILENAME "cgenius.cfg"

class CSettings {
public:
	// NOTE: If no option structure is passed, you must be no means use
	// 	loadGameCfg(), saveGameCfg(), loadDefaultGameCfg(); setOption(..);
	// Those depend on that structure.
	CSettings(stOption *p_option=NULL);
	short saveDrvCfg();
	bool loadDrvCfg();
	bool loadGameCfg();
	void saveGameCfg();
	void loadDefaultGameCfg();
	void setOption( int opt, const std::string &name, char value);
	virtual ~CSettings();
	
private:
	stOption *mp_option;

	bool checkOptionPtr();
};

#endif /* CSETTINGS_H_ */
