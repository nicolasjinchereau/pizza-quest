/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <vector>
#include "Math.h"
#include "Object.h"
#include "PQB2Shapes.h"
#include "Sprite.h"
#include "Sound.h"
#include <Box2D.h>
#include "Texture.h"
#include "RigidBody.h"


enum PQ_GAMETYPES
{
	PQ_MAP_OBJECT,
	RES_BASE,
	RES_IMAGE,
	RES_SOUND,
	MAP_BASE,
	MAP_IMAGE,
	MAP_SOUND,
	MAP_GRAPH_NODE
};

class PQGame;

/*****************************
PQ RESOURCE
*****************************/

class PQResource : public Object
{
public:
	
	PQResource();
	
	// READ FROM FILE
	char source_file[260];
	char resource_name[32];
	char class_id[32];
	/////////////////

	virtual void Init(){}
};

/*****************************
PQ RESOURCE IMAGE
*****************************/

class PQResImage : public PQResource
{
public:
	PQResImage();
	~PQResImage();

	virtual void Init();

	// READ FROM FILE
	uint16_t nRows;
	uint16_t nCols;
	///////////////////
	
	vector<shared_ptr<PQB2Shape>> collision_shapes;
	shared_ptr<Texture> tex;
};

/*****************************
PQ RESOURCE SOUND
*****************************/

class PQResSound : public PQResource
{
public:
	PQResSound();

	// READ FROM FILE
	// no resource properties
	/////////////////

	shared_ptr<Sound> snd;

	virtual void Init();
};

/*****************************
PQ MAP OBJECT
*****************************/

class PQMapObject : public Object
{
public:
	PQMapObject();
	virtual ~PQMapObject();

	// READ FROM FILE
	uint8_t resIndex; // not set until this node is being written to file
	vec2f position;
	/////////////////
	
	shared_ptr<PQResource> resource;
};

/*****************************
PQ MAP IMAGE
*****************************/

class PQMapImage : public PQMapObject
{
	PQMapImage(const PQMapImage &other){}
public:
	PQMapImage();
	virtual ~PQMapImage();

	// READ FROM FILE
	uint8_t row;
	uint8_t col;
	float angle;
	float scale;
	int16_t value1;
	int16_t value2;
	/////////////////

	shared_ptr<Sprite> img;
	shared_ptr<RigidBody> body;

	shared_ptr<PQResImage> GetResource();
	virtual void Start() override;
};

/*****************************
PQ MAP SOUND
*****************************/

class PQMapSound : public PQMapObject
{
public:
	// READ FROM FILE
	uint8_t volume;
	bool loop;
	uint16_t delay;
	uint16_t triggerRadius;
	////////

	PQResSound *GetResource();
	
	virtual void Play(const Rect &viewRect){}
};