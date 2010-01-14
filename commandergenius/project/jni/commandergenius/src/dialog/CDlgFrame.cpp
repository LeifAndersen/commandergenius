/*
 * CDlgFrame.cpp
 *
 *  Created on: 20.08.2009
 *      Author: gerstrong
 */

#include "CDlgFrame.h"
#include "../graphics/CGfxEngine.h"

CDlgFrame::CDlgFrame(int x, int y, int w, int h, Uint8 theme) {
	
	m_8x8tileheight = 8;
	m_8x8tilewidth = 8;
	
	m_x = x;
	m_y = y;
	m_w = w*m_8x8tilewidth;
	m_h = h*m_8x8tileheight;
	
	m_theme = theme;
}

void CDlgFrame::draw(SDL_Surface *dst)
{
	if(m_theme == DLG_THEME_OLDSCHOOL)
	{
		drawOldSchoolFrame(dst);
	}
	else if(m_theme == DLG_THEME_RED)
	{
		drawRedFrame(dst);
	}
}

void CDlgFrame::drawOldSchoolFrame(SDL_Surface *dst)
{
	CFont &Font = g_pGfxEngine->getFont();
	// However it should be blit to a memory map and then rendered every time. If not, no new effects can
	// be improved.
	
	// first draw the blank rect
	int i, j;
	for(j = 0 ; j < m_h - m_8x8tileheight ; j+= m_8x8tileheight )
	{
		for(i = 0 ; i < m_w - m_8x8tilewidth ; i+= m_8x8tilewidth )
			Font.drawCharacter(dst, 32, m_x + i, m_y + j); // 32 is a blank tile
		Font.drawCharacter(dst, 32,m_x + m_w - m_8x8tilewidth, m_y + j); // for the last tile
	}
	
	// then the borders
	for( i = m_8x8tilewidth ; i < m_w-m_8x8tilewidth ; i+= m_8x8tilewidth )
	{
		Font.drawCharacter(dst, 2, m_x + i, m_y); // 2 is one upper-border
		Font.drawCharacter(dst, 7, m_x + i, m_y + m_h - m_8x8tileheight); // 7 is also the lower-border
	}
	Font.drawCharacter(dst, 2, m_x + m_w - m_8x8tilewidth, m_y); // for the last tile
	Font.drawCharacter(dst, 2, m_x + m_w - m_8x8tilewidth, m_y + m_h - m_8x8tileheight); // for the last tile
	for( j = m_8x8tileheight ; j < m_h-m_8x8tileheight ; j+= m_8x8tileheight )
	{
		Font.drawCharacter(dst, 4, m_x, m_y + j ); // 4 is one left-border
		Font.drawCharacter(dst, 5, m_x + m_w - m_8x8tilewidth, m_y + j ); // 5 is the right-border
	}
	
	// At last the corners
	Font.drawCharacter(dst, 1, m_x, m_y ); // Upper-Left corner
	Font.drawCharacter(dst, 3, m_x + m_w - m_8x8tilewidth, m_y ); // Upper-Right corner
	Font.drawCharacter(dst, 6, m_x, m_y + m_h - m_8x8tileheight ); // Lower-Left corner
	Font.drawCharacter(dst, 8, m_x + m_w - m_8x8tilewidth, m_y + m_h - m_8x8tileheight ); // Lower-Right corner
}

void CDlgFrame::drawRedFrame(SDL_Surface *dst)
{
	CFont &Font = g_pGfxEngine->getFont();
	// However it should be blit to a memory map and then rendered every time. If not, no new effects can
	// be improved.
	
	// first draw the blank rect
	int i, j;
	for(j = 0 ; j < m_h - m_8x8tileheight ; j+= m_8x8tileheight )
	{
		for(i = 0 ; i < m_w - m_8x8tilewidth ; i+= m_8x8tilewidth )
			Font.drawCharacter(dst, 32, m_x + i, m_y + j); // 32 is a blank tile
		Font.drawCharacter(dst, 32,m_x + m_w - m_8x8tilewidth, m_y + j); // for the last tile
	}
	
	// then the borders
	for( i = m_8x8tilewidth ; i < m_w-m_8x8tilewidth ; i+= m_8x8tilewidth )
	{
		Font.drawCharacter(dst, 24, m_x + i, m_y); // 2 is one upper-border
		Font.drawCharacter(dst, 29, m_x + i, m_y + m_h - m_8x8tileheight); // 7 is also the lower-border
	}
	Font.drawCharacter(dst, 24, m_x + m_w - m_8x8tilewidth, m_y); // for the last tile
	Font.drawCharacter(dst, 24, m_x + m_w - m_8x8tilewidth, m_y + m_h - m_8x8tileheight); // for the last tile
	for( j = m_8x8tileheight ; j < m_h-m_8x8tileheight ; j+= m_8x8tileheight )
	{
		Font.drawCharacter(dst, 26, m_x, m_y + j ); // 4 is one left-border
		Font.drawCharacter(dst, 27, m_x + m_w - m_8x8tilewidth, m_y + j ); // 5 is the right-border
	}
	
	// At last the corners
	Font.drawCharacter(dst, 23, m_x, m_y ); // Upper-Left corner
	Font.drawCharacter(dst, 25, m_x + m_w - m_8x8tilewidth, m_y ); // Upper-Right corner
	Font.drawCharacter(dst, 28, m_x, m_y + m_h - m_8x8tileheight ); // Lower-Left corner
	Font.drawCharacter(dst, 30, m_x + m_w - m_8x8tilewidth, m_y + m_h - m_8x8tileheight ); // Lower-Right corner
}

CDlgFrame::~CDlgFrame() {
	// TODO Auto-generated destructor stub
}
