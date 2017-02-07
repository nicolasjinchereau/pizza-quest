/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "Audio.h"
#include <AL/al.h>
#include <AL/alc.h>

Audio::Audio()
{
	device = NULL;
	context = NULL;
	alive = false;
}

Audio::~Audio()
{
	_terminate();
}

bool Audio::_initialize()
{
	{
		unique_lock<mutex>(m);

		// open the audio device
		device = alcOpenDevice(NULL);
		if(!device)
			return false;

		// create a context
		context = alcCreateContext(device, NULL);
		alcMakeContextCurrent(context);
	
		// clear error code
		alGetError();
		
		alListener3f(AL_POSITION, 0.0, 0.0, 1.0);
		alListener3f(AL_VELOCITY, 0.0, 0.0, 0.0);

		ALfloat orientation[] = {0.0, 0.0, -1.0, 0.0, 1.0, 0.0};
		alListenerfv(AL_ORIENTATION, orientation);
	}

	alive = true;



	return true;
}

void Audio::_terminate()
{
	if(alive)
	{
		{
			unique_lock<mutex>(m);
			alcDestroyContext(context);
			alcCloseDevice(device);

			context = NULL;
			device = NULL;

			alive = false;
		}
	}
}

bool Audio::Initialize()
{
	return that->_initialize();
}

void Audio::Terminate()
{
	that->_terminate();
}

bool Audio::Alive()
{
	return that->alive;
}

unique_lock<mutex> Audio::GetLock()
{
	return unique_lock<mutex>(that->m);
}
