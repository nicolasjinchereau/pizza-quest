/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <cstdlib>
#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <AL/al.h>
#include <AL/alc.h>
#include "MP3Decoder.h"
#include "Trace.h"

using namespace std;

class Stream
{
	static const int BUFFER_COUNT = 8;
	static const int BUFFER_SIZE = 8192;
private:
	static void BufferCallback(void *param);

	volatile bool waitDone;
	volatile bool alive;
	ALuint source;
	bool looping;

	mutex m;
	condition_variable cv;
	MP3Decoder decoder;
	ALuint buffers[BUFFER_COUNT];
	thread _playerThread;

	void _loadBuffers();
	void _unloadBuffers();

public:

	Stream();
	~Stream();

	bool Open(const char *filename);
	void Close();

	void Play();
	void Pause();
	void Stop();

	bool IsPlaying();
	bool IsPaused();
	bool IsStopped();
	bool IsOpen();
	
	void SetLoop(bool loop = false);
	void SetGain(float gain = 1.0f);
	float GetGain();

private:

	virtual void OnDestroy();
	virtual void OnInitialize();
	friend class Engine;
};
