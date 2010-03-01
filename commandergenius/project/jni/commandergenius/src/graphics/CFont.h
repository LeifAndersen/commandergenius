/*
 * CFont.h
 *
 *  Created on: 26.08.2009
 *      Author: gerstrong
 *
 *  This class is the has the graphics for fonts of any type.
 *  They can be read out of EgaGraphicstile, or a special image file.
 *  The surface has not like the orignal keen game 256 tiles.
 *  It can carry up to 512 tiles, so 256 are free for personal stuff.
 *  One thing which is used, is the special twirl for all angles.
 *  That missed in the original game and made the graphics less smooth
 */

#ifndef CFONT_H_
#define CFONT_H_

#include <SDL.h>
#include <string>

enum{
	LETTER_TYPE_NORMAL,
	LETTER_TYPE_RED,
	LETTER_TYPE_BLUE,
	LETTER_TYPE_INVERSE,
	LETTER_TYPE_DISABLED
};

class CFont {
public:
	CFont();

	bool CreateSurface(SDL_Color *Palette, Uint32 Flags);
	bool optimizeSurface();
	SDL_Surface *getSDLSurface() { return m_FontSurface; }

	bool loadHiColourFont( const std::string& filename );
	void generateGlowFonts();
	void generateSpecialTwirls();
	void generateInverseFonts();
	void generateDisabledFonts();

	void drawTwirl(SDL_Surface* dst, int twirlframe, Uint16 x, Uint16 y);

	void drawCharacter(SDL_Surface* dst, Uint16 character, Uint16 xoff, Uint16 yoff);
	void drawFont(SDL_Surface* dst, const std::string& text, Uint16 xoff, Uint16 yoff, Uint8 lettertype = LETTER_TYPE_NORMAL);

	void DestroySurface();
	virtual ~CFont();

private:
	SDL_Surface *m_FontSurface;
};

#endif /* CFONT_H_ */
