/*
 * COpenGL.cpp
 *
 *  Created on: 04.06.2009
 *      Author: gerstrong
 */
#ifdef USE_OPENGL

#include "COpenGL.h"
#include "CVideoDriver.h"
#include "../CLogFile.h"

// gamerect is the base resolution for the game which is scaled with the filter
// depending on what resolution has been chosen, it is mostly 320x200 or 320x240
COpenGL::COpenGL(SDL_Rect &gamestdrect) :
m_blitsurface(NULL),
m_Depth(4),	// 32-bit colour depth is default
m_GameStdRect(gamestdrect)
{ }

bool COpenGL::initGL(float Width, float Height, unsigned char Depth,
					 GLint oglfilter, unsigned char scalex, float aspect)
{
	m_Depth = Depth;
	m_ScaleX = scalex;
	m_aspectratio = Width/Height;
	//m_texparam = GL_TEXTURE_RECTANGLE_ARB;
	m_texparam = GL_TEXTURE_2D;
	
	// Set the proper resolution for OpenGL. Very important, when user changes the resolution
	if(aspect == 4.0f/3.0f)
	{
	if(m_aspectratio < 4.0f/3.0f)
		glViewport(0,(Height-((Width/320)*240))/2,Width, (Width/320)*240);
	else if(m_aspectratio > 3.0f/2.0f)
		glViewport((Width-((Height/240)*320))/2,0, (Height/240)*320, Height);
	else
		glViewport(0,0,Width, Height);
	}
	else if(aspect == 3.0f/2.0f)
	{
			if(m_aspectratio < 3.0f/3.0f)
		glViewport(0,(Height-((Width/720)*480))/2,Width, (Width/720)*480);
	else if(m_aspectratio > 3.0f/2.0f)
		glViewport((Width-((Height/480)*720))/2,0, (Height/480)*720, Height);
	else
		glViewport(0,0,Width, Height);
	}
	else if(aspect == 8.0f/5.0f)
	{
	if(m_aspectratio < 8.0f/5.0f)
		glViewport(0,(Height-((Width/320)*200))/2,Width, (Width/320)*200);
	else if(m_aspectratio > 8.0f/5.0f)
		glViewport((Width-((Height/200)*320))/2,0, (Height/200)*320, Height);
	else
		glViewport(0,0,Width, Height);
	}
	else if(aspect == 16.0f/9.0f)
	{
			if(m_aspectratio < 16.0f/9.0f)
		glViewport(0,(Height-((Width/320)*180))/2,Width, (Width/320)*180);
	else if(m_aspectratio > 16.0f/9.0f)
		glViewport((Width-((Height/180)*320))/2,0, (Height/180)*320, Height);
	else
		glViewport(0,0,Width, Height);
	}
	
	// Set clear colour
	glClearColor(0,0,0,0);
	
	// Set projection
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0 , 1, 1, 0 , -1 , 1 );
	
	// Now Initialize modelview matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	// Enable Texture loading for the blit screen
	/*  uint textureID;
	 
	 glEnable (GL_TEXTURE_2D);
	 glGenTextures(1, &textureID);
	 glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textureID);
	 //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	 glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	 glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	 glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	 glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	 */
	
    /*Using the standard OpenGL API for specifying a 2D texture
	 image: glTexImage2D, glSubTexImage2D, glCopyTexImage2D,
	 and glCopySubTexImage2D.  The target for these commands is
	 GL_TEXTURE_RECTANGLE_ARB though.
	 
	 This is similar to how the texture cube map functionality uses the 2D
	 texture image specification API though with its own texture target.
	 
	 The texture target GL_TEXTURE_RECTANGLE_ARB should also
	 be used for glGetTexImage, glGetTexLevelParameteriv, and
	 glGetTexLevelParameterfv.*/
	
	
	glEnable (m_texparam);
	glBindTexture(m_texparam, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	glTexParameteri(m_texparam, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (m_texparam, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	
	glTexParameteri (m_texparam, GL_TEXTURE_MAG_FILTER, oglfilter);
	glTexParameteri (m_texparam, GL_TEXTURE_MIN_FILTER, oglfilter);
	
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	
	if(scalex > 1)
		m_opengl_buffer = new char[m_GameStdRect.h*m_GameStdRect.w*scalex*Depth];
	else
		m_opengl_buffer = NULL;
	
	// If there were any errors
	int error;
	error = glGetError();
	if( error != GL_NO_ERROR)
	{
		g_pLogFile->ftextOut("OpenGL Init(): %d<br>",error);
		return false;
	}
	else
	{
		g_pLogFile->ftextOut("OpenGL Init(): Interface succesfully opened!<br>");
	}
	
	return true;
}

void COpenGL::setSurface(SDL_Surface *blitsurface)
{	m_blitsurface = blitsurface; }

void COpenGL::render(void)
{
	//Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);		// Clear The Screen
	
	LockSurface(m_blitsurface);
	
	if(m_ScaleX == 2) //Scale 2x
	{
		scale(2, m_opengl_buffer, (m_GameStdRect.w<<1)*(m_Depth>>3), m_blitsurface->pixels,
				m_GameStdRect.w*(m_Depth>>3), (m_Depth>>3), m_GameStdRect.w, m_GameStdRect.h);
		
		glTexImage2D(m_texparam, 0, GL_RGBA, m_GameStdRect.w<<1, m_GameStdRect.h<<1, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_opengl_buffer);
		//glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, GAME_STD_WIDTH<<1, m_GameStdRect.h<<1, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_opengl_buffer);
		//glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA8, GAME_STD_WIDTH<<1, m_GameStdRect.h<<1, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_opengl_buffer);
	}
	else if(m_ScaleX == 3) //Scale 3x
	{
		scale(3, m_opengl_buffer, (m_GameStdRect.w*3)*(m_Depth>>3), m_blitsurface->pixels,
			  m_GameStdRect.w*(m_Depth>>3), (m_Depth>>3), m_GameStdRect.w, m_GameStdRect.h);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_GameStdRect.w*3, m_GameStdRect.h*3, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_opengl_buffer);
	}
	else if(m_ScaleX == 4) //Scale 4x
	{
		scale(4, m_opengl_buffer, (m_GameStdRect.w<<2)*(m_Depth>>3), m_blitsurface->pixels,
			  m_GameStdRect.w*(m_Depth>>3), (m_Depth>>3), m_GameStdRect.w, m_GameStdRect.h);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_GameStdRect.w<<2, m_GameStdRect.h<<2, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_opengl_buffer);
	}
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_GameStdRect.w, m_GameStdRect.h, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_blitsurface->pixels);
	
	
	glBindTexture (GL_TEXTURE_2D, 1);
	
	glBegin (GL_QUADS);
	glTexCoord2f (0.0, 0.0);
	glVertex3f (0.0, 0.0, 0.0);
	glTexCoord2f (1.0, 0.0);
	glVertex3f (1.0, 0.0, 0.0);
	glTexCoord2f (1.0, 1.0);
	glVertex3f (1.0, 1.0, 0.0);
	glTexCoord2f (0.0, 1.0);
	glVertex3f (0.0, 1.0, 0.0);
	glEnd();
	
	UnlockSurface(m_blitsurface);
	
	// Reset (Position?)
	glLoadIdentity();
	
	SDL_GL_SwapBuffers();
}

COpenGL::~COpenGL() {
	if(m_opengl_buffer){ delete[] m_opengl_buffer; m_opengl_buffer = NULL; }
}

#endif
