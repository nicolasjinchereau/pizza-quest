/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "bytestream.h"
#include <memory>

using namespace std;

class Wave
{
	enum Format
	{
		PCM = 1, // only supported format
		FLOAT = 3,
		MULAW = 6,
		ALAW = 7,
		IBM_MULAW = 257,
		IBM_ALAW = 258,
		ADPCM = 259
	};

	struct Header
	{
		char     id[4];     // "RIFF" or "RF64"
		uint32_t size;      // header chunk Size or -1(0xFFFFFFFF) if there is a 'ds64' chunk
		char     type[4];   // "WAVE"
	};

	struct ChunkInfo
	{
		char     type[4];
		uint32_t size;
	};
	
	struct JunkChunk
	{
		// dummy bytes
	};

	struct FormatChunk
	{
		unsigned short      AudioFormat;    // WAVE_FORMAT (1 means uncompressed)
		unsigned short      NumOfChan;      // Number of channels 1-5
		unsigned long       SamplesPerSec;  // Sampling Frequency in Hz
		unsigned long       bytesPerSec;    // bytes per second
		unsigned short      blockAlign;     // 2 = 16-bit mono, 4 = 16-bit stereo
		unsigned short      bitsPerSample;  // Number of bits per sample
		// more data may be appended to this chunk, see MS website.
	};

	struct FactChunk // only used for compressed files
	{
		unsigned long dwNumSamples;  // number of audio frames;
	};

	struct DataChunk
	{
		unsigned long size;
		unique_ptr<unsigned char[]> data;
	};
	
	bool ChunkCmp(const char type[4], const char *str);

	Header header;
	FormatChunk format;
	FactChunk fact;
	DataChunk data;

public:
	Wave();
	Wave(const char *filename);
	~Wave();

	unsigned long DataSize();
	unsigned long Frequency();
	unsigned short Bitrate();
	unsigned short Channels();
	void *Data();

	bool Open(const char *filename);
	void Close();

	//Header header;
	//unique_ptr<unsigned char> data;
};
