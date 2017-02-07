/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "Stream.h"
#include "Audio.h"

void Stream::BufferCallback(void *param)
{
	Stream *stream = (Stream*)param;

	stream->waitDone = true;
	stream->cv.notify_one();
}

Stream::Stream()
{
	source = 0;
	alive = true;
	waitDone = false;
	looping = false;

	memset(buffers, 0, sizeof(ALuint) * BUFFER_COUNT);
}

Stream::~Stream()
{
	Close();
}

void Stream::_loadBuffers()
{
	decoder.Rewind();

	short sampleBuffer[BUFFER_SIZE];

	int i;
	for(i = 0; i < BUFFER_COUNT; i++)
	{
		int nSamples = decoder.GetSamples(sampleBuffer, BUFFER_SIZE);
		
		if(nSamples > 0)
		{
			alBufferData(buffers[i],
						 AL_FORMAT_STEREO16,
						 sampleBuffer,
						 nSamples * sizeof(short),
						 decoder.SampleRate());
		}
	}
	
	if(i <= 0)
	{
		cout << "could not initialize buffers, not enough data in file." << endl;
		return;
	}

	alSourceQueueBuffers(source, i, buffers);
}

void Stream::_unloadBuffers()
{
	ALuint buffer;
	ALint queuedBuffers;

	alGetSourcei(source, AL_BUFFERS_QUEUED, &queuedBuffers);

	for(int i = 0; i < queuedBuffers; i++)
		alSourceUnqueueBuffers(source, 1, &buffer);
}

bool Stream::Open(const char *filename)
{
	Close();
	
	auto lk_audio = Audio::GetLock();

	if(!decoder.Open(filename))
		return false;
	
	ALfloat orientation[] = {0.0, 0.0, 1.0, 0.0, 1.0, 0.0};

	alGenSources(1, &source);
	alGenBuffers(BUFFER_COUNT, buffers);

	alSource3f(source, AL_POSITION, 0.0, 0.0, 0.0);
	alSource3f(source, AL_VELOCITY, 0.0, 0.0, 0.0);
	alSourcefv(source, AL_DIRECTION, orientation);
	alSourcef(source, AL_ROLLOFF_FACTOR, 0.0);
	alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);
	alSourceSetBufferCallback(source, &BufferCallback, this);

	alive = true;

	_playerThread = thread([&]()
	{
		while(alive)
		{
			auto lk = Audio::GetLock();
			//unique_lock<mutex> lkWait(m);

			waitDone = false;
			cv.wait(lk, [&](){ return waitDone; });

			if(!alive)
				break;

			int processed;
			alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

			while(processed > 0)
			{
				--processed;

				ALuint buffer;
				alSourceUnqueueBuffers(source, 1, &buffer);

				short sampleBuffer[BUFFER_SIZE];

				int nSamples = decoder.GetSamples(sampleBuffer, BUFFER_SIZE);
				
				if(nSamples > 0)
				{
					alBufferData(buffer,
									AL_FORMAT_STEREO16,
									sampleBuffer,
									nSamples * sizeof(short),
									decoder.SampleRate());
					
					alSourceQueueBuffers(source, 1, &buffer);
				}
				else if(nSamples == 0)
				{
					// stream has ended, cannot get more samples,
					// and all buffers has been played
					ALint nQueued;
					alGetSourcei(source, AL_BUFFERS_QUEUED, &nQueued);

					if(nQueued == 0)
					{
						Trace("stream ended");
						processed = 0;

						if(looping)
						{
							_loadBuffers();
							alSourcePlay(source);
						}

						break;
					}
				}
				else if(nSamples == -1)
				{
					// an error has occured while trying to decode the stream
					Trace("stream failed to update");
					break;
				}
			}
		}
	});

	return true;
}

void Stream::Close()
{
	{
		auto lk = Audio::GetLock();
		if(!alIsSource(source))
			return;

		alSourceStop(source);

		alive = false;
		waitDone = true;
		cv.notify_one();
	}

	_playerThread.join();
	
	{
		auto lk = Audio::GetLock();
		alDeleteSources(1, &source);
		alDeleteBuffers(BUFFER_COUNT, buffers);
	}

	decoder.Close();

	source = 0;
	memset(buffers, 0, sizeof(ALuint) * BUFFER_COUNT);
}

void Stream::Play()
{
	auto lk = Audio::GetLock();

	if(!alIsSource(source)) return;

	ALenum state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);

	if(state == AL_STOPPED || state == AL_INITIAL)
		_loadBuffers();

	alSourcePlay(source);
}

void Stream::Pause()
{
	auto lk = Audio::GetLock();

	if(!alIsSource(source)) return;
	alSourcePause(source);
}

void Stream::Stop()
{
	auto lk = Audio::GetLock();

	if(!alIsSource(source)) return;

	ALenum state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);

	if(state == AL_PLAYING)
	{
		alSourceStop(source);
		_unloadBuffers();
	}
}

bool Stream::IsPlaying()
{
	auto lk = Audio::GetLock();

	if(!alIsSource(source)) return false;

	ALenum state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	return (state == AL_PLAYING);
}

bool Stream::IsPaused()
{
	auto lk = Audio::GetLock();

	if(!alIsSource(source)) return false;

	ALenum state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	return (state == AL_PAUSED);
}

bool Stream::IsStopped()
{
	auto lk = Audio::GetLock();

	if(!alIsSource(source)) return false;

	ALenum state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	return (state == AL_STOPPED || state == AL_INITIAL);
}

bool Stream::IsOpen()
{
	return alive;
}

void Stream::SetLoop(bool loop)
{
	looping = loop;
}

void Stream::SetGain(float gain)
{
	auto lk = Audio::GetLock();

	if(!alIsSource(source)) return;
	alSourcef(source, AL_GAIN, gain);
}

float Stream::GetGain()
{
	auto lk = Audio::GetLock();

	if(!alIsSource(source)) return 0.0f;

	ALfloat _gain;
	alGetSourcef(source, AL_GAIN, &_gain);

	return _gain;
}

void Stream::OnDestroy()
{
	auto lk = Audio::GetLock();

	if(alive)
	{
		ALenum state;
		alGetSourcei(source, AL_SOURCE_STATE, &state);
		if(state == AL_PLAYING)
			alSourcePause(source);
	}
}

void Stream::OnInitialize()
{
	auto lk = Audio::GetLock();

	if(alive)
	{
		ALenum state;
		alGetSourcei(source, AL_SOURCE_STATE, &state);
		if(state == AL_PAUSED)
			alSourcePlay(source);
	}
}
