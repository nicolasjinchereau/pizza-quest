/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include <afxwin.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "list.h"
#include "definitions.h"
#include "PQMapNode.h"
#include "filename.h"
#include "vecmath.h"
#include "History.h"
#include "PQString.h"
#include "Ruler.h"

using namespace std;

const Uint8 PQ_MAP_FILE_MIN_VERSION = 105;
const Uint8 PQ_MAP_FILE_MAX_VERSION = 143;

#define MAX_PATH_LENGTH 260
#define MAX_RESOURCES 300

enum EDITORLAYERS
{
	LAYER_SPRITES,
	LAYER_PEDGRAPH,
	LAYER_VEHGRAPH,
	LAYER_RULERS,
};

class ResourceList : public vector<PQResourcePtr>, public Undoable
{
public:
	ResourceList(){}
	ResourceList(ResourceList &that)
	{
		*this = that;
	}
	ResourceList &operator=(ResourceList &that)
	{
		this->clear();

		for(iterator it = that.begin(); it != that.end(); it++)
			this->push_back(*it);

		return *this;
	}

	virtual Undoable *GetCopy()	{return new ResourceList(*this);}
	virtual void Swap(Undoable *object)
	{
		ResourceList &obj = *(ResourceList*)object;
		ResourceList tmp = *this;
		*this = obj;
		obj = tmp;
	}
};

class MapNodeList : public list<PQMapNodePtr>, public Undoable
{
public:
	MapNodeList(){}
	MapNodeList(MapNodeList &that)
	{
		*this = that;
	}
	
	MapNodeList &operator=(MapNodeList &that)
	{
		this->clear();

		for(iterator it = that.begin(); it != that.end(); it++)
			this->push_back(*it);

		return *this;
	}

	virtual Undoable *GetCopy() {return new MapNodeList(*this);}
	virtual void Swap(Undoable *object)
	{
		MapNodeList &obj = *(MapNodeList*)object;
		MapNodeList tmp = *this;
		*this = obj;
		obj = tmp;
	}
};

class GraphNodeList : public vector<PQGraphNodePtr>, public Undoable
{
public:
	GraphNodeList(){}
	GraphNodeList(GraphNodeList &that)
	{
		*this = that;
	}

	GraphNodeList &operator=(GraphNodeList &that)
	{
		this->clear();
		this->reserve( that.size() );

		for(size_t i = 0; i < that.size(); i++)
			this->push_back( that[i] );

		return *this;
	}
	virtual Undoable *GetCopy()
	{
		return new GraphNodeList(*this);
	}
	virtual void Swap(Undoable *object)
	{
		GraphNodeList &obj = *(GraphNodeList*)object;
		GraphNodeList tmp = *this;
		*this = obj;
		obj = tmp;
	}
};

typedef MapNodeList SelectionSet;

class PQMap
{
public:

	static Filename filename;
	static bool hasChanged;
	static int currentLayer;

	ResourceList resources;
	MapNodeList scene;
	GraphNodeList pedGraph;
	GraphNodeList vehGraph;

	SelectionSet sceneSelection;
	SelectionSet pedGraphSelection;
	SelectionSet vehGraphSelection;

	unsigned int timeLimit;
	int numDeliveries;
	
	VRuler vRuler[2];
	HRuler hRuler[2];

///////////////////////

	PQMap();
	virtual ~PQMap();

	void Initialize();

	int SaveMap(Filename fname);
	int LoadMap(Filename fname);
	Filename FindFile(const Filename &filename, const Filename &path);

	void CloseMap();
	std::string GetFilename();
	void SetChanged();
	bool HasChanged();
	int NumResources();
	
	bool AddResource(const PQResourcePtr &res);
	bool RemoveResource(const PQResourcePtr &res);
	bool RemoveUnusedResources();

	void AddNode(PQMapNodePtr &node);
	void RemoveNode(PQMapNodePtr &node);

	PQMapNodePtr NodeFromPoint(float x, float y);
	bool SelectNode(PQMapNodePtr &node);
	void DeselectNode(PQMapNodePtr &node);
	int SelectAll();
	int DeselectAll();
	int SelectRect(Rect<float> &rect);
	int DeleteSelection();
	void MoveSelection(float offset_x, float offset_y);
	bool ScaleSelectionUp();
	bool ScaleSelectionDown();

	bool ConnectSelectedNodes();
	bool BreakAllConnections();
	bool BreakSelectedPairs();
	bool ToggleDestination();

	bool SelBringForward();
	int SelBringToFront();
	bool SelSendBack();
	int SelSendToBack();
	SelectionSet &GetSelection();
	//MapNodeList &GetNodeList();
	void SetCurrentLayer(int layer);
	int GetCurrentLayer();
	
	void Draw(GL_Screen *dest, Rect<float> &rect);
	void DrawRulers(GL_Screen *dest, const Rect<float> &viewRect);
};