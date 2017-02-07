/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include <mad.h>
#include <fstream>
#include <cassert>

using namespace std;

#define MAD_INPUT_BUFFER_SIZE (5 * 8192)

#define MAD_LEFT_CHAN 0
#define MAD_RIGHT_CHAN 1

class MP3Decoder
{
public:
	MP3Decoder();
	~MP3Decoder();
	
	int Open(const char *filename);
	void Close();

	void Rewind();

	int GetSamples(short *sampleBuffer, int maxSamples);

	unsigned int SampleRate();
	unsigned short SamplesPerChannel();
	unsigned short Channels();

private:
	
	void _initMAD();
	void _finishMAD();
	short _fixedToShort(mad_fixed_t Fixed);

	ifstream fin;
	unsigned char inputBuffer[MAD_INPUT_BUFFER_SIZE + MAD_BUFFER_GUARD];
	mad_stream stream;
	mad_frame frame;
	mad_synth synth;
	mad_timer_t timer;
	int synthSampleOffset;
	int synthSampleCount;
};
