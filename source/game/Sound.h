/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include "Object.h"
#include "Wave.h"
using namespace std;

typedef unsigned int ALuint;

class Sound : public Object
{
public:
	Sound();
	Sound(const string &filename);
	~Sound();

	bool Open(const string &filename);
	void Close();

	void Play();
	void Stop();
	void Pause();
	
	bool IsPlaying();
	bool IsPaused();
	bool IsStopped();
	bool IsOpen();
	float length();

	void pitch(float setPitch);
	float pitch() const;

	void SetLoop(bool loop = false);
	void SetGain(float gain = 1.0f);
	void SetBlocking(bool enable = false);

private:
	Wave wave;
	bool opened;
	bool blocking;
	float gain;

	virtual void OnDestroy();
	virtual void OnInitialize();

	ALuint buffer;
	ALuint source;
	float _length;
};
