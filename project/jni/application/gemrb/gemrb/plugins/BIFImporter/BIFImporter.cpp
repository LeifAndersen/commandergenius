/* GemRB - Infinity Engine Emulator
 * Copyright (C) 2003 The GemRB Project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 */

#include "BIFImporter.h"

#include "win32def.h"

#include "Compressor.h"
#include "FileCache.h"
#include "Interface.h"
#include "PluginMgr.h"
#include "System/SlicedStream.h"
#include "System/FileStream.h"

BIFImporter::BIFImporter(void)
{
	stream = NULL;
	fentries = NULL;
	tentries = NULL;
}

BIFImporter::~BIFImporter(void)
{
	if (stream) {
		delete( stream );
	}
	if (fentries) {
		delete[] fentries;
	}
	if (tentries) {
		delete[] tentries;
	}
}

bool BIFImporter::DecompressBIF(DataStream* compressed, const char* path)
{
	print( "Decompressing\n" );
	if (!core->IsAvailable( PLUGIN_COMPRESSION_ZLIB ))
		return false;
	PluginHolder<Compressor> comp(PLUGIN_COMPRESSION_ZLIB);
	ieDword unCompBifSize;
	compressed->ReadDword( &unCompBifSize );
	print( "\nDecompressing file: [..........]" );
	fflush(stdout);
	FileStream out;
	if (!out.Create(path)) {
		printMessage("BIFImporter", "Cannot write %s.\n", RED, path);
		return false;
	}
	ieDword finalsize = 0;
	int laststep = 0;
	while (finalsize < unCompBifSize) {
		ieDword complen, declen;
		compressed->ReadDword( &declen );
		compressed->ReadDword( &complen );
		if (comp->Decompress( &out, compressed, complen ) != GEM_OK) {
			return false;
		}
		finalsize = out.GetPos();
		if (( int ) ( finalsize * ( 10.0 / unCompBifSize ) ) != laststep) {
			laststep++;
			print( "\b\b\b\b\b\b\b\b\b\b\b" );
			int l;

			for (l = 0; l < laststep; l++)
				print( "|" );
			for (; l < 10; l++)//l starts from laststep
				print( "." );
			print( "]" );
			fflush(stdout);
		}
	}
	print( "\n" );
	return true;
}

int BIFImporter::OpenArchive(const char* filename)
{
	if (stream) {
		delete( stream );
		stream = NULL;
	}
	FileStream* file = FileStream::OpenFile(filename);
	if( !file) {
		return GEM_ERROR;
	}
	char Signature[8];
	if (file->Read(Signature, 8) == GEM_ERROR) {
		delete file;
		return GEM_ERROR;
	}
	delete file;
	//normal bif, not in cache
	if (strncmp( Signature, "BIFFV1  ", 8 ) == 0) {
		stream = CacheFile( filename );
		if (!stream)
			return GEM_ERROR;
		stream->Read( Signature, 8 );
		strcpy( path, filename );
		ReadBIF();
		return GEM_OK;
	}
	//not found as normal bif
	//checking compression type
	FileStream* compressed = FileStream::OpenFile( filename );
	if (!compressed)
		return GEM_ERROR;
	compressed->Read( Signature, 8 );
	if (strncmp( Signature, "BIF V1.0", 8 ) == 0) {
		ieDword fnlen, complen, declen;
		compressed->ReadDword( &fnlen );
		char* fname = ( char* ) malloc( fnlen );
		compressed->Read( fname, fnlen );
		strlwr(fname);
		compressed->ReadDword( &declen );
		compressed->ReadDword( &complen );
		print( "Decompressing\n" );
		stream = CacheCompressedStream(compressed, fname, complen);
		free( fname );
		delete( compressed );
		if (!stream)
			return GEM_ERROR;
		stream->Read( Signature, 8 );
		if (strncmp( Signature, "BIFFV1  ", 8 ) == 0)
			ReadBIF();
		else
			return GEM_ERROR;
		return GEM_OK;
	}

	if (strncmp( Signature, "BIFCV1.0", 8 ) == 0) {
		//print("'BIFCV1.0' Compressed File Found\n");
		PathJoin( path, core->CachePath, compressed->filename, NULL );
		if (file_exists(path)) {
			//print("Found in Cache\n");
			delete( compressed );
			stream = FileStream::OpenFile(path);
			if (!stream)
				return GEM_ERROR;
			stream->Read( Signature, 8 );
			if (strncmp( Signature, "BIFFV1  ", 8 ) == 0) {
				ReadBIF();
			} else
				return GEM_ERROR;
			return GEM_OK;
		}
		if (!DecompressBIF(compressed, path)) {
			delete( compressed );
			return GEM_ERROR;
		}
		delete( compressed );
		stream = FileStream::OpenFile(path);
		if (!stream)
			return GEM_ERROR;
		stream->Read( Signature, 8 );
		if (strncmp( Signature, "BIFFV1  ", 8 ) == 0)
			ReadBIF();
		else
			return GEM_ERROR;
		return GEM_OK;
	}
	delete (compressed);
	return GEM_ERROR;
}

DataStream* BIFImporter::GetStream(unsigned long Resource, unsigned long Type)
{
	if (Type == IE_TIS_CLASS_ID) {
		unsigned int srcResLoc = Resource & 0xFC000;
		for (unsigned int i = 0; i < tentcount; i++) {
			if (( tentries[i].resLocator & 0xFC000 ) == srcResLoc) {
				return SliceStream( stream, tentries[i].dataOffset,
							tentries[i].tileSize * tentries[i].tilesCount );
			}
		}
	} else {
		ieDword srcResLoc = Resource & 0x3FFF;
		for (ieDword i = 0; i < fentcount; i++) {
			if (( fentries[i].resLocator & 0x3FFF ) == srcResLoc) {
				return SliceStream( stream, fentries[i].dataOffset,
							fentries[i].fileSize );
			}
		}
	}
	return NULL;
}

void BIFImporter::ReadBIF(void)
{
	ieDword foffset;
	stream->ReadDword( &fentcount );
	stream->ReadDword( &tentcount );
	stream->ReadDword( &foffset );
	stream->Seek( foffset, GEM_STREAM_START );
	fentries = new FileEntry[fentcount];
	tentries = new TileEntry[tentcount];
	if (!fentries || !tentries) {
		if (fentries) {
			delete fentries;
			fentries = NULL;
		}
		if (tentries) {
			delete tentries;
			tentries = NULL;
		}
		return;
	}
	unsigned int i;

	for (i=0;i<fentcount;i++) {
		stream->ReadDword( &fentries[i].resLocator);
		stream->ReadDword( &fentries[i].dataOffset);
		stream->ReadDword( &fentries[i].fileSize);
		stream->ReadWord( &fentries[i].type);
		stream->ReadWord( &fentries[i].u1);
	}
	for (i=0;i<tentcount;i++) {
		stream->ReadDword( &tentries[i].resLocator);
		stream->ReadDword( &tentries[i].dataOffset);
		stream->ReadDword( &tentries[i].tilesCount);
		stream->ReadDword( &tentries[i].tileSize);
		stream->ReadWord( &tentries[i].type);
		stream->ReadWord( &tentries[i].u1);
	}
}

#include "plugindef.h"

GEMRB_PLUGIN(0xC7F133C, "BIF File Importer")
PLUGIN_CLASS(IE_BIF_CLASS_ID, BIFImporter)
END_PLUGIN()
