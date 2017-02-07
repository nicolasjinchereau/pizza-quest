/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "MP3Decoder.h"
#include <iostream>
using namespace std;

MP3Decoder::MP3Decoder()
{
	mad_synth_init(&synth);
}

MP3Decoder::~MP3Decoder()
{
	Close();
}

void MP3Decoder::_initMAD()
{
	mad_stream_init(&stream);
	mad_frame_init(&frame);
	mad_synth_init(&synth);
	mad_timer_reset(&timer);

	synthSampleOffset = 0;
	synthSampleCount = 0;
}

void MP3Decoder::_finishMAD()
{
	mad_synth_finish(&synth);
	mad_synth_init(&synth);
	mad_frame_finish(&frame);
	mad_stream_finish(&stream);	
}

int MP3Decoder::Open(const char *filename)
{
	Close();

	fin.open(filename, ios_base::binary);

	if(!fin.is_open())
	{
		fin.clear();
		cout << "MP3Decoder Could not open file:" << filename << endl;
		return 0;
	}

	_initMAD();

	return 1;
}

void MP3Decoder::Close()
{
	if(fin.is_open())
	{
		fin.close();
		fin.clear();
		_finishMAD();
	}
}

void MP3Decoder::Rewind()
{
	if(fin.is_open())
	{
		fin.clear();
		fin.seekg(0);

		_finishMAD();
		_initMAD();
	}
}

unsigned int MP3Decoder::SampleRate()
{
	return synth.pcm.samplerate;
}

unsigned short MP3Decoder::SamplesPerChannel()
{
	return synth.pcm.length;
}

unsigned short MP3Decoder::Channels()
{
	return synth.pcm.channels;
}

int MP3Decoder::GetSamples(short *sampleBuffer, int maxSamples)
{
	int nSamples = 0;

	while(nSamples < maxSamples)
	{
		if(synthSampleOffset < synthSampleCount) // output all PCM samples
		{
			sampleBuffer[nSamples++] = _fixedToShort( synth.pcm.samples[MAD_LEFT_CHAN][synthSampleOffset] );

			if(MAD_NCHANNELS(&frame.header) == 2)
				sampleBuffer[nSamples++] = _fixedToShort( synth.pcm.samples[MAD_RIGHT_CHAN][synthSampleOffset] );

			++synthSampleOffset;
		}
		else
		{
			if(stream.error == MAD_ERROR_BUFLEN || stream.buffer == NULL) // refill input buffer from file
			{
				if(fin.eof())
					break;

				unsigned char *readStart = NULL;
				int bReadSize = 0;
				int bLeftOver = 0;

				if(stream.next_frame != NULL)
				{
					// move leftovers to front of buffer, and fill
					bLeftOver = stream.bufend - stream.next_frame;
					memmove(inputBuffer, stream.next_frame, bLeftOver);
					readStart = inputBuffer + bLeftOver;
					bReadSize = MAD_INPUT_BUFFER_SIZE - bLeftOver;
				}
				else
				{
					//fill entire buffer
					bLeftOver = 0;
					readStart = inputBuffer;
					bReadSize = MAD_INPUT_BUFFER_SIZE;
				}

				// refill input buffer and set stream buffer pointers
				if( bReadSize > 0 && !fin.eof() )
				{
					fin.read((char*)readStart, bReadSize);
					unsigned int bytesRead = (unsigned int)fin.gcount();

					if( fin.eof() )
					{
						// if this is the last frame, MAD_BUFFER_GUARD bytes of 0s must be
						// appended to the buffer, otherwise libmad will not be able to decode it.
						memset(readStart + bytesRead, 0, MAD_BUFFER_GUARD);
						bytesRead += MAD_BUFFER_GUARD;
					}

					// set stream buffer pointers
					mad_stream_buffer(&stream, inputBuffer, bLeftOver + bytesRead);
					stream.error = MAD_ERROR_NONE;
				}
			}

			// decode 1 frame and synthesize PCM samples
			if(mad_frame_decode(&frame, &stream) == 0)
			{
				mad_synth_frame(&synth, &frame);
				mad_timer_add(&timer, frame.header.duration);

				synthSampleOffset = 0;
				synthSampleCount = synth.pcm.length;
			}
			else
			{
				if(MAD_RECOVERABLE(stream.error) || stream.error == MAD_ERROR_BUFLEN)
				{
					continue;
				}
				else
				{
					cout << "critical error, cannot decode file" << endl;
					return -1;
				}
			}
		}
	}

	return nSamples;
}

short MP3Decoder::_fixedToShort(mad_fixed_t Fixed)
{
	if(Fixed >= MAD_F_ONE)
		return SHRT_MAX;

	if(Fixed <= -MAD_F_ONE)
		return SHRT_MIN;

	return (short)(Fixed >> (MAD_F_FRACBITS - 15));
}
