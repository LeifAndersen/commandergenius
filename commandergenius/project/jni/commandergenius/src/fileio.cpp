/* FILEIO.C
 Functions responsible for loading data from files, such as the one that
 decodes the level map files (loadmap()) and the one that loads in the
 tile attribute data contained in ep?attr.dat (loadtileattributes()).
 The functions for loading the graphics (EGALATCH&EGASPRIT) are in latch.c.
 */

#include <unistd.h>
#include <sys/types.h>
#include <vector>
#include <stdio.h>

#include "keen.h"
#include "sdl/CVideoDriver.h"
#include "sdl/sound/CSound.h"
#include "fileio.h"
#include "include/fileio/rle.h"
#include "CLogFile.h"
#include "StringUtils.h"
#include "Debug.h"
#include "FindFile.h"

#ifdef TARGET_WIN32
#define uint unsigned int
#define ulong unsigned long
#endif

unsigned int fgeti(FILE *fp) {
	unsigned int lsb, msb;
	lsb = fgetc(fp);
	msb = fgetc(fp);
	return (msb<<8) | lsb;
}

void fputi(uint word, FILE *fp)
{
	fputc(word&255, fp);
	fputc(word/256, fp);
}

unsigned long fgetl(FILE *fp) {
	unsigned int temp1, temp2, temp3, temp4;
	temp1 = fgetc(fp);
	temp2 = fgetc(fp);
	temp3 = fgetc(fp);
	temp4 = fgetc(fp);
	return (temp4<<24) | (temp3<<16) | (temp2<<8) | temp1;
}

void fputl(unsigned long word, FILE *fp)
{
	unsigned long a,b,c,d;
	a=b=c=d = word;
	a &= 0xFF000000; a >>= 24;
	b &= 0x00FF0000; b >>= 16;
	c &= 0x0000FF00; c >>= 8;
	d &= 0x000000FF;
	fputc(d, fp);
	fputc(c, fp);
	fputc(b, fp);
	fputc(a, fp);
}

// returns a pointer to the string with name 'name'
std::string getstring(const std::string& name)
{
	for(int i=0;i<numStrings;i++)
	{
		if (name == strings[i].name)
		{
			// Returning is not enough anymore
			// Convert the /h and /H into proper colours
			// This is necessary since the new graphics engine
			return strings[i].stringptr;
		}
	}
	
	return "UNKNOWN '" + name + "' STRING";
}

// because windows and linux read files differently, these is to function to get them correctly
std::string formatPathString(const std::string& path)
{
	std::string buffer = path;
	if( buffer.size() > 0 && buffer[buffer.size()-1] != '/' )
		buffer += '/';
	return buffer;
}
