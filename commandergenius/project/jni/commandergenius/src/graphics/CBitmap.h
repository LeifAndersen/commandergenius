/*
 * CBitmap.h
 *
 *  Created on: 03.09.2009
 *      Author: gerstrong
 */

#ifndef CBITMAP_H_
#define CBITMAP_H_

#include <SDL.h>
#include <vector>
#include <string>

class CBitmap {
public:
	CBitmap();

	bool createSurface(Uint32 flags, SDL_Color *Palette);
	bool optimizeSurface();

	SDL_Surface *getSDLSurface();
	void setDimensions(Uint16 w, Uint16 h);
	void setName(const std::string &name);
	void setColorPalette(SDL_Color *Palette);
	Uint16 getWidth() { return m_width; }
	Uint16 getHeight() { return m_height; }
	std::string getName() { return m_name; }

	void draw(SDL_Surface *dst, Uint16 x, Uint16 y);

	void destroySurface();
	virtual ~CBitmap();

private:
	Uint16 m_width;
	Uint16 m_height;
	std::string m_name;
	SDL_Surface *m_BitmapSurface;
};

#endif /* CBITMAP_H_ */
