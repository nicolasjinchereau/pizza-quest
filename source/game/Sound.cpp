/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "Sound.h"
#include "Engine.h"
#include "Audio.h"
#include <AL/al.h>
#include <AL/alc.h>

Sound::Sound()
{
	blocking = false;
	opened = false;
	gain = 1.0f;

	buffer = 0;
	source = 0;
	_length = 0;
}

Sound::Sound(const string &filename)
{
	blocking = false;
	opened = false;
	gain = 1.0f;

	buffer = 0;
	source = 0;
	_length = 0;

	Open(filename);
}

Sound::~Sound()
{
	Close();
}

bool Sound::Open(const string &filename)
{
	if(!wave.Open(filename.c_str()))
	{
		Trace("could not open file", filename);
		return false;
	}

	auto lk = Audio::GetLock();

	ALenum format;
	ALfloat sourceOri[] = {0.0, 0.0, 1.0, 0.0, 1.0, 0.0};

	// clear error before loading sounds
	alGetError();

// create a buffer
	alGenBuffers(1, &buffer);

	if(wave.Channels() == 2 && wave.Bitrate() == 8)
	{
		format = AL_FORMAT_STEREO8;
	}
	else if(wave.Channels() == 2 && wave.Bitrate() == 16)
	{
		format = AL_FORMAT_STEREO16;
	}
	else if(wave.Channels() == 1 && wave.Bitrate() == 8)
	{
		format = AL_FORMAT_MONO8;
	}
	else if(wave.Channels() == 1 && wave.Bitrate() == 16)
	{
		format = AL_FORMAT_MONO16;
	}
	else
	{
		Trace("Unsupported Audio Format: ", filename);
		return false;
	}

	// fill the buffer with data
	alBufferData(buffer, format, wave.Data(), wave.DataSize(), wave.Frequency());
	
	// generate an audio source
	alGenSources(1, &source);
	
	// attach the buffer to the source
	alSourcei(source, AL_BUFFER, buffer);

	// set source position, velocity and orientation
	alSource3f(source, AL_POSITION, 0.0, 0.0, 0.0f);
	alSource3f(source, AL_VELOCITY, 0.0, 0.0, 0.0);
	alSourcefv(source, AL_DIRECTION, sourceOri);

	// disable the affect of distance on volume
	alSourcef(source, AL_ROLLOFF_FACTOR, 0.0);
	alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);

	float bytesPerFrame = (float)(wave.Channels() * wave.Bitrate() / 8);
	float bytesPerSecond = wave.Frequency() * bytesPerFrame;
	_length = (float)wave.DataSize() / bytesPerSecond;

	opened = true;

	return true;
}

void Sound::Close()
{
	auto lk = Audio::GetLock();

	if(opened == false)
		return;

	if(alIsSource(source))
	{
		alSourceStop(source);
		alSourcei(source, AL_BUFFER, 0);
		alDeleteSources(1, &source);
		source = 0;
	}

	wave.Close();
	
	if(alIsBuffer(buffer))
	{
		alDeleteBuffers(1, &buffer);
		buffer = 0;
	}

	opened = false;
	_length = 0;
}

void Sound::Play()
{
	auto lk = Audio::GetLock();

	ALenum state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);

	// stop the sound if it's already playing
	if(state == AL_PLAYING && !blocking)
	{
		alSourceStop(source);
	}
	
	// play the sound
	alSourcePlay(source);
}

void Sound::Stop()
{
	auto lk = Audio::GetLock();
	if(source) alSourceStop(source);
}

void Sound::Pause()
{
	auto lk = Audio::GetLock();
	if(source) alSourcePause(source);
}

bool Sound::IsPlaying()
{
	auto lk = Audio::GetLock();

	ALenum state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	return (state == AL_PLAYING);
}

bool Sound::IsPaused()
{
	auto lk = Audio::GetLock();

	ALenum state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	return (state == AL_PAUSED);
}

bool Sound::IsStopped()
{
	auto lk = Audio::GetLock();

	ALenum state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	return (state == AL_INITIAL || state == AL_STOPPED);
}

bool Sound::IsOpen()
{
	return opened;
}

float Sound::length()
{
	return _length;
}

void Sound::pitch(float setPitch)
{
	auto lk = Audio::GetLock();
	if(source) alSourcef(source, AL_PITCH, setPitch);
}

float Sound::pitch() const
{
	float ret = 0;
	
	auto lk = Audio::GetLock();
	if(source) alGetSourcef(source, AL_PITCH, &ret);

	return ret;
}

void Sound::SetLoop(bool loop)
{
	auto lk = Audio::GetLock();
	if(source) alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
}

void Sound::SetGain(float gain)
{
	auto lk = Audio::GetLock();
	alSourcef(source, AL_GAIN, gain);
}

void Sound::SetBlocking(bool enable)
{
	blocking = enable;
}

void Sound::OnInitialize()
{
	auto lk = Audio::GetLock();

	if(opened)
	{
		ALenum state;
		alGetSourcei(source, AL_SOURCE_STATE, &state);

		if(state == AL_PAUSED)
			alSourcePlay(source);
	}
}

void Sound::OnDestroy()
{
	auto lk = Audio::GetLock();

	if(opened)
	{
		ALenum state;
		alGetSourcei(source, AL_SOURCE_STATE, &state);

		if(state == AL_PLAYING)
			alSourcePause(source);
	}
}
