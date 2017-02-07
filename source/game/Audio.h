/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <mutex>
#include <memory>
#include "Singleton.h"
#include "Stream.h"
#include "Sound.h"

using namespace std;

typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;

class Audio : public Singleton<Audio>
{
private:

	ALCdevice *device;
	ALCcontext *context;
	bool alive;
	mutex m;

	friend class Engine;
	friend class Sound;
	friend class Stream;

	bool _initialize();
	void _terminate();
public:

	Audio();
	~Audio();

	static bool Initialize();
	static void Terminate();
	static bool Alive();
	static unique_lock<mutex> GetLock();
};
