/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#ifndef INC_PQMAPNODE_H
#define INC_PQMAPNODE_H

#include "definitions.h"
#include <assert.h>
#include "GL_Screen.h"
#include "GL_Sprite.h"
#include "GL_Draw.h"
#include <vector>
#include <string>
#include <fstream>
#include "filename.h"
#include "list.h"
#include "resource.h"
#include "PQB2Shapes.h"
#include <memory>
#include "History.h"
#include <algorithm>

using namespace std;

// do not remove
// DEPRECATED MAP NODE IDs
enum CHUNKTYPE
{
	RES_BASE,
	RES_IMAGE,
	RES_SOUND,
	MAP_BASE,
	MAP_IMAGE,
	MAP_SOUND,
	NODE_BASE = 255
};

enum NEW_CHUNKTYPE
{
	N_NODE_BASE,
	N_RES_BASE,
	N_RES_IMAGE,
	N_RES_SOUND,
	N_MAP_BASE,
	N_MAP_IMAGE,
	N_MAP_SOUND,
	N_GRAPH_NODE_BASE,
	N_PED_GRAPH_NODE,
	N_VEH_GRAPH_NODE,
};

class PQNode;
class PQResource;
class PQResImage;
class PQResSound;
class PQMapNode;
class PQMapImage;
class PQMapSound;
class PQGraphNode;
class PQPedGraphNode;
class PQVehGraphNode;

typedef shared_ptr<PQNode>			PQNodePtr;
typedef shared_ptr<PQResource>		PQResourcePtr;
typedef shared_ptr<PQResImage>		PQResImagePtr;
typedef shared_ptr<PQResSound>		PQResSoundPtr;
typedef shared_ptr<PQMapNode>		PQMapNodePtr;
typedef shared_ptr<PQMapImage>		PQMapImagePtr;
typedef shared_ptr<PQMapSound>		PQMapSoundPtr;
typedef shared_ptr<PQGraphNode>		PQGraphNodePtr;
typedef shared_ptr<PQPedGraphNode>	PQPedGraphNodePtr;
typedef shared_ptr<PQVehGraphNode>	PQVehGraphNodePtr;

typedef shared_ptr<GL_Sprite> GL_SpritePtr;


/*****************************
PQ NODE
*****************************/

class PQNode : public Undoable
{
public:
	PQNode(Uint8 type = N_NODE_BASE);
	PQNode(const PQNode &that);
	virtual ~PQNode();
	
	// WRITTEN TO FILE
	Uint8 type;
	/////////////////

	virtual long GetDataSize() = 0;
	virtual Undoable *GetCopy() = 0;
	virtual void Swap(Undoable *object) = 0;

};

/*****************************
PQ RESOURCE
*****************************/

class PQResource : public PQNode
{
public:
	
	//WRITTEN TO FILE
	char source_file[260];
	char resource_name[32];
	char class_id[32];
	/////////////////

	// temporarily holds this resource's index while snapshotting editor
	int tmpIndex;

	PQResource(Uint8 type = N_RES_BASE);
	PQResource(const PQResource &that);
	virtual ~PQResource();
	
	void SetResourceName(const char *resource_name);
	void SetClassID(const char *class_id);
	
	////////////////////////////
	virtual void SetSourceFile(const char *sourceFile) = 0;
	virtual long GetDataSize() = 0;
	virtual void Draw(GL_Screen *dest, float x, float y, Uint8 Row, Uint8 Col, float Scale, float Angle, float shade = 1.0f, float alpha = 1.0f) = 0;
	virtual void DrawThumb(GL_Screen *dest) = 0;

	virtual Undoable *GetCopy() = 0;
	virtual void Swap(Undoable *object);
};

/*****************************
PQ RES IMAGE
*****************************/

class PQResImage : public PQResource
{
public:
	//WRITTEN TO FILE
	Uint16 nRows;
	Uint16 nCols;
	vector<PQB2ShapePtr> collision_shapes;
	///////////////////

	PQResImage(string sourceFile = "", string resourceName = "", string classID = "", Uint16 nRows = 1, Uint16 nCols = 1);
	PQResImage(const PQResImage &that);
	virtual ~PQResImage();
	
	virtual void SetSourceFile(const char *sourceFile);
	virtual long GetDataSize();
	virtual void Draw(GL_Screen *dest, float x, float y, Uint8 Row, Uint8 Col, float Scale, float Angle, float shade = 1.0f, float alpha = 1.0f);
	virtual void DrawThumb(GL_Screen *dest);

	virtual Undoable *GetCopy();
	virtual void Swap(Undoable *object);

	// resource image
	GL_SpritePtr img;

	// all the properties given out to new nodes
	Uint8 newRow;
	Uint8 newCol;
	float newAngle;
	float newScale;
	Int16 newValue1;
	Int16 newValue2;
};

/*****************************
PQ RES SOUND
*****************************/

class PQResSound : public PQResource
{
public:
	// WRITTEN TO FILE
	// NO SOUND BASE PROPERTIES
	/////////////////

	PQResSound(string sourceFile = "", string resourceName = "", string classID = "");
	PQResSound(const PQResSound &that);
	~PQResSound();

	virtual void SetSourceFile(const char *sourceFile);
	virtual long GetDataSize();
	virtual void Draw(GL_Screen *dest, float x, float y, Uint8 Row, Uint8 Col, float Scale, float Angle, float shade = 1.0f, float alpha = 1.0f);
	virtual void DrawThumb(GL_Screen *dest);
	
	virtual Undoable *GetCopy();
	virtual void Swap(Undoable *object);

	// image of a speaker for drawing thumbnail
	static GL_SpritePtr img;

	// resource sound
	// not implemented yet

	// all the properties given out to new nodes
	float newVolume;
	bool newLoop;
	int newDelay;
	int newTriggerRadius;
};

/*****************************
PQ MAP NODE
*****************************/

class PQMapNode : public PQNode
{
public:
	
	//WRITTEN TO FILE
	Uint8 resIndex; // not set until this node is being written to file
	Vec2<float> position;
	/////////////////

	// temporaries for editor use
	PQResourcePtr res; // resIndex is written to file instead of this pointer
	Rect<float> rect;
	bool selected;

	PQMapNode(Uint8 type = N_MAP_BASE);
	PQMapNode(const PQMapNode &that);
	virtual ~PQMapNode(){}

	virtual void CalcRect() = 0;
	virtual long GetDataSize() = 0;
	virtual void Draw(GL_Screen *dest, float offset_x = 0.0f, float offset_y = 0.0f, float shade = 1.0f, float alpha = 1.0f) = 0;
	virtual void DrawThumb(GL_Screen *dest) = 0;
	virtual void SetPosition(float x, float y) = 0;
	virtual void Move(float offset_x, float offset_y) = 0;

	virtual PQMapNodePtr Copy() = 0;

	virtual Undoable *GetCopy() = 0;
	virtual void Swap(Undoable *object);
};

/*****************************
PQ MAP IMAGE
*****************************/

class PQMapImage : public PQMapNode
{
public:
	
	//WRITTEN TO FILE
	Uint8 row;
	Uint8 col;
	float angle;
	float scale;
	Int16 value1; // values that are passed to the behavior class in the game
	Int16 value2;
	/////////////////

	PQMapImage(PQResourcePtr &resource);
	PQMapImage(const PQMapImage &that);
	virtual ~PQMapImage();
	
	void SetPosition(const Vec2<float> &position);

	virtual void CalcRect();
	virtual long GetDataSize();
	virtual void Draw(GL_Screen *dest, float offset_x = 0.0f, float offset_y = 0.0f, float shade = 1.0f, float alpha = 1.0f);
	virtual void DrawThumb(GL_Screen *dest);
	virtual void SetPosition(float x, float y);
	virtual void Move(float offset_x, float offset_y);
	virtual PQMapNodePtr Copy();

	virtual Undoable *GetCopy();
	virtual void Swap(Undoable *object);
};

/*****************************
PQ MAP SOUND
*****************************/

class PQMapSound : public PQMapNode
{
public:
	//WRITTEN TO FILE
	//Uint8 volume;
	//bool loop;
	//Uint16 delay;
	//Uint16 triggerRadius;

	float volume;
	bool loop;
	int delay;
	int triggerRadius;
	/////////////////

	PQMapSound(PQResourcePtr &resource);
	PQMapSound(const PQMapSound &that);
	virtual ~PQMapSound();

	void SetPosition(const Vec2<float> &position);

	virtual void CalcRect();
	virtual long GetDataSize();
	virtual void Draw(GL_Screen *dest, float offset_x = 0.0f, float offset_y = 0.0f, float shade = 1.0f, float alpha = 1.0f);
	virtual void DrawThumb(GL_Screen *dest);
	virtual void SetPosition(float x, float y);
	virtual void Move(float offset_x, float offset_y);
	virtual PQMapNodePtr Copy();

	virtual Undoable *GetCopy();
	virtual void Swap(Undoable *object);
};

/*****************************
PQ GRAPH NODE
*****************************/

class PQGraphNode : public PQMapNode
{
public:
	//WRITTEN TO FILE
	vector<PQGraphNodePtr> neighbours;
	/////////////////
	
	// only valid while snapshotting editor
	int tmpIndex;
	const float radius;
	bool isDestination;

	PQGraphNode(float x = 0, float y = 0, Uint8 type = N_GRAPH_NODE_BASE);
	PQGraphNode(const PQGraphNode &that);
	virtual ~PQGraphNode();

	void TraceNeighbours(GL_Screen *dest, const Rect<float> &viewRect);
	bool AddNeighbour(PQGraphNodePtr &neighbour);
	bool RemoveNeighbour(PQGraphNodePtr &neighbour);
	bool DisconnectAll();
	bool DisconnectSelected();

	virtual void CalcRect();
	virtual long GetDataSize() = 0;
	virtual void Draw(GL_Screen *dest, float offset_x = 0.0f, float offset_y = 0.0f, float shade = 1.0f, float alpha = 1.0f){}
	virtual void Draw(GL_Screen *dest, const Rect<float> &viewRect, float shade = 1.0f, float alpha = 1.0f) = 0;
	virtual void DrawThumb(GL_Screen *dest){};
	virtual void SetPosition(float x, float y);
	virtual void Move(float offset_x, float offset_y);
	virtual PQMapNodePtr Copy() = 0;

	virtual Undoable *GetCopy() = 0;
	virtual void Swap(Undoable *object);
};

/*****************************
PQ PEDESTRIAN GRAPH NODE
*****************************/

class PQPedGraphNode : public PQGraphNode
{
public:
	PQPedGraphNode(float x = 0, float y = 0);
	PQPedGraphNode(const PQPedGraphNode &that);
	virtual ~PQPedGraphNode();
	
	virtual long GetDataSize();
	virtual void Draw(GL_Screen *dest, const Rect<float> &viewRect, float shade = 1.0f, float alpha = 1.0f);
	virtual PQMapNodePtr Copy();

	virtual Undoable *GetCopy();
	virtual void Swap(Undoable *object);
};

/*****************************
PQ VEHICLE GRAPH NODE
*****************************/

class PQVehGraphNode : public PQGraphNode
{
public:
	PQVehGraphNode(float x = 0, float y = 0);
	PQVehGraphNode(const PQVehGraphNode &that);
	virtual ~PQVehGraphNode();

	virtual long GetDataSize();
	virtual void Draw(GL_Screen *dest, const Rect<float> &viewRect, float shade = 1.0f, float alpha = 1.0f);
	virtual PQMapNodePtr Copy();

	virtual Undoable *GetCopy();
	virtual void Swap(Undoable *object);
};

typedef list<PQMapNodePtr>::iterator	  MNodeIter;
typedef vector<PQGraphNodePtr>::iterator  GNodeIter;

#endif /* INC_PQMAPNODE_H */