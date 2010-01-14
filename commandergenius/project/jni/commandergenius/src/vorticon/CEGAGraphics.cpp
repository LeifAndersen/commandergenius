/*
 * CEGAGraphics.cpp
 *
 *  Created on: 11.07.2009
 *      Author: gerstrong
 */

#include "CEGAGraphics.h"
#include "../sdl/CVideoDriver.h"

#ifdef TARGET_WIN32
#include <dir.h>
#endif
#include <fstream>
#include <vector>
#include "../StringUtils.h"
#include "../FindFile.h"

using namespace std;

#define SAFE_SDL_FREE(x) if(x) { SDL_FreeSurface(x); x = NULL; }

CEGAGraphics::CEGAGraphics(short episode, const std::string& path) {
	m_episode = episode;
	m_path = path;
	
	// EGAHEAD Structure
	LatchPlaneSize = 0;
	SpritePlaneSize = 0;
	BitmapTableStart = 0;
	SpriteStart = 0;
	
	FontTiles = 0;
	FontLocation = 0;
	ScreenTiles = 0;
	ScreenLocation = 0;
	Num16Tiles = 0;
	Tiles16Location = 0;
	NumBitmaps = 0;
	BitmapLocation = 0;
	NumSprites = 0;
	SpriteLocation = 0;
	compressed = 0;
	
	m_Latch = NULL;
	m_Sprit = NULL;
	m_FontSurface = NULL;
	m_BigTileSurface = NULL;
	m_TileSurface = NULL;
	m_BitmapsSurface = NULL;
}

bool CEGAGraphics::loadData( int version, unsigned char *p_exedata )
{
	std::string buf;
	vector<char> databuf;
	
	// assure that the last used resources are freed
	g_pGfxEngine->freeFonts();
	g_pGfxEngine->freeTilemap();
	
	// Set the palette, so the proper colours are loaded
	g_pGfxEngine->Palette.setupColorPalettes();
	
	if(m_path == "")
		buf = "egahead.ck" + itoa(m_episode);
	else
		buf = m_path + "/egahead.ck" + itoa(m_episode);
	std::ifstream HeadFile; OpenGameFileR(HeadFile, buf, ios::binary);
	
	if(!HeadFile)
		return false;
	
	char byte;
	while(!HeadFile.eof())
	{
		HeadFile.read(&byte,1);
		databuf.push_back(byte);
	}
	HeadFile.close();
	
	char *data = new char[databuf.size()];
	memcpy(data, &databuf[0], databuf.size());
	
	// Now copy the data to the EGAHEAD Structure
    memcpy(&LatchPlaneSize,data,4);
    memcpy(&SpritePlaneSize,data+4,4);
    memcpy(&BitmapTableStart,data+8,4);
    memcpy(&SpriteStart,data+12,4);
	
    memcpy(&FontTiles,data+16,2);
    memcpy(&FontLocation,data+18,4);
    memcpy(&ScreenTiles,data+22,2);
    memcpy(&ScreenLocation,data+24,4);
    memcpy(&Num16Tiles,data+28,2);
    memcpy(&Tiles16Location,data+30,4);
    memcpy(&NumBitmaps,data+34,4);
    memcpy(&BitmapLocation,data+36,4);
    memcpy(&NumSprites,data+40,4);
    memcpy(&SpriteLocation,data+42,4);
    memcpy(&compressed,data+46,4);
	
    m_Latch = new CEGALatch(LatchPlaneSize,
							BitmapTableStart,
							FontTiles,
							FontLocation,
							ScreenTiles,
							ScreenLocation,
							Num16Tiles,
							Tiles16Location,
							NumBitmaps,
							BitmapLocation);
	
    m_Latch->loadHead( data, m_episode);
	
    m_Latch->loadData( m_path, m_episode, version, p_exedata, (compressed>>1)); // The second bit tells, if latch is compressed.
	
	
    m_Sprit = new CEGASprit(SpritePlaneSize,
							SpriteStart,
							NumSprites,
							SpriteLocation,
							m_path);
    m_Sprit->loadHead(data);
	
    if(m_path == "")
		buf = "egasprit.ck" + itoa(m_episode);
	else
		buf = m_path + "/egasprit.ck" + itoa(m_episode);
    m_Sprit->loadData(buf,(compressed>>1));
	
    delete[] data;
	
    return true;
}

int CEGAGraphics::getNumSprites() { return NumSprites; }

CEGAGraphics::~CEGAGraphics() {
	if(m_Latch) delete m_Latch;
	if(m_Sprit) delete m_Sprit;
	m_Latch = NULL;
	m_Sprit = NULL;

	SAFE_SDL_FREE(m_FontSurface);
	SAFE_SDL_FREE(m_BigTileSurface);
	SAFE_SDL_FREE(m_TileSurface);
	SAFE_SDL_FREE(m_BitmapsSurface);
}


