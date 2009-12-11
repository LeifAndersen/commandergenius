/*
 * CExeFile.cpp
 *
 *  Created on: 17.07.2009
 *      Author: gerstrong
 */

#include "CExeFile.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include "../StringUtils.h"
#include "../FindFile.h"
#include "../CLogFile.h"

using namespace std;

CExeFile::CExeFile(int episode, const std::string& datadirectory) {
	m_episode = episode;
	m_datadirectory = datadirectory;
	if( m_datadirectory != "") if(*(m_datadirectory.end()-1) != '/') m_datadirectory += "/";
	m_data = NULL;

	crc32_init();
}

CExeFile::~CExeFile() {
	if(m_data) delete[] m_data; m_data = NULL;
}

bool CExeFile::readData()
{
	std::string filename =  m_datadirectory + "keen" + itoa(m_episode) + ".exe";

	std::ifstream File; OpenGameFileR(File, filename, ios::binary);

	if(!File)
	{
		g_pLogFile->textOut(RED,"Error the executable \"" + filename + "\" is missing!");
		return false;
	}

	File.seekg(0,ios::end);
	m_datasize = File.tellg();
	File.seekg(0,ios::beg);

	unsigned char * m_data_temp = new unsigned char[m_datasize];
	File.read((char*)m_data_temp, m_datasize);

	File.close();

	vector<unsigned char> decdata;

	if(unlzexe(m_data_temp, &decdata))
	{
		m_datasize = decdata.size();
		m_data = new unsigned char[m_datasize];
		memcpy(m_data, &decdata[0], m_datasize);
	}
	else
	{
		m_datasize -= 512; // if already decompressed subtract the header
		m_data = new unsigned char[m_datasize];
		memcpy(m_data, m_data_temp+512,m_datasize);
	}
	delete[] m_data_temp;

	m_crc = getcrc32( m_data, m_datasize );

	// TODO: Why is printf used here! Please change that to g_pLogFile->Textout
    printf( "EXE processed with size of %d and crc of %X\n", m_datasize, m_crc );

	return true;
}

int CExeFile::get_bit(int *p_bit_count, unsigned char *fin, int *posin)
{
	static unsigned short bits = 0;
	int bit = bits & 1;
	(*p_bit_count)--;

	if ((*p_bit_count) <= 0)
	{
		unsigned short a,b;
		a = (unsigned char) fin[(*posin)++];
		b = (unsigned char) fin[(*posin)++] << 8;
		bits = a | b;

		if ((*p_bit_count) == -1) /* special case for first bit word */
		{
			bit = bits & 1;
			bits >>= 1;
		}

		(*p_bit_count) += 16;
	}
	else
		bits >>= 1;

	return bit;
}

// return how much was unpacked or zero if nothing was unpacked
int CExeFile::unlzexe(unsigned char *fin, vector<unsigned char> *outbuffer)
{
	short offset=0;
	int repeat;
	int posin = 0;	// position of input

	int pos = 0;
	int bit_count = 0;

	/* skip header */
	posin = 32;

	while (1)
	{
		if (get_bit(&bit_count, fin, &posin))
		{
			outbuffer->push_back(fin[posin]);
			pos++;
			posin++;
		}
		else
		{
			if (get_bit(&bit_count, fin, &posin))
			{
				unsigned char tmp[2];
				memcpy(tmp,fin+posin,2);
				posin+=2;
				repeat = (tmp[1] & 0x07);

				offset = ((tmp[1] & ~0x07) << 5) | tmp[0] | 0xE000;

				if (repeat == 0)
				{
					repeat = fin[posin++];

					if (repeat == 0)
						break;
					else if (repeat == 1)
						continue;
					else
						repeat++;
				}
				else
					repeat += 2;
			}
			else
			{
				repeat = get_bit(&bit_count, fin, &posin) << 1;
				repeat |= get_bit(&bit_count, fin, &posin);
				repeat += 2;
				offset = fin[posin++] | 0xFF00;
			}

			while (repeat > 0)
			{
				outbuffer->push_back(outbuffer->at(pos + offset));
				pos++;
				repeat--;
			}
		}
	}

	return pos;
}

int CExeFile::getEXEVersion()
{
    switch (m_datasize)
    {
		case 99762:
			if(m_episode != 1)
				return -1;
			else
				return 110;
		case 99972:
			if(m_episode != 1)
				return -1;
			else
				return 131;
		case 398:
			if(m_episode != 1)
				return -1;
			else
				return 134;

		case 118114:
			if(m_episode != 2)
				return -1;
			else
				return 100;
		case 118160:
			if(m_episode != 2)
				return -1;
			else
				return 131;

		case 127086:
			if(m_episode != 3)
				return -1;
			else
				return 100;
		case 127104:
			if(m_episode != 3)
				return -1;
			else
				return 131;

		default: return -2;
    }
}

int CExeFile::getEXECrc()
{
    int version = getEXEVersion();

    switch( m_episode )
    {
        case 1:
            switch( version )
		{
			case 110:
				if(m_crc != 1)
					return -1;
				else
					return 1;
			case 131:
				if(m_crc != 0x195771AE)
					return -1;
				else
					return true;
			case 134:
				if(m_crc != 1)
					return -1;
				else
					return 1;
			default: return -2;
		}
        case 2:
            switch( version )
		{
			case 100:
				if(m_crc != 1)
					return -1;
				else
					return 1;
			case 131:
				if(m_crc != 0x94E464B4)
					return -1;
				else
					return true;
		}
        case 3:
            switch( version )
		{
			case 100:
				if(m_crc != 1)
					return -1;
				else
					return 1;
			case 131:
				if(m_crc != 0x70D3264D)
					return -1;
				else
					return 1;
		}
		default: return -2;
    }
}

unsigned char* CExeFile::getData()
{	return m_data;	}
