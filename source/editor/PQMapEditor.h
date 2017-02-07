/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include "definitions.h"
#include <afxwin.h>
#include "PQMapNode.h"
#include <vector>
#include "GL_Draw.h"
#include "filename.h"
#include <iostream>
#include <fstream>
#include <string>
#include "list.h"
#include <sstream>
#include "ResourceDialog.h"
#include <algorithm>
#include <memory>
#include "vecmath.h"
#include "PQMap.h"

using namespace std;

#define MIN_GRID_SIZE 10
#define MAX_GRID_SIZE 256
#define MIN_ZOOM 0.1f
#define MAX_ZOOM 5.0f

enum EDITORTOOLS
{
	TOOL_SELECTION,
	TOOL_CREATION,
};

class PQMapEditor
{
public:

	PQMapEditor();
	~PQMapEditor();

	int LoadMap(const Filename &filename);
	int SaveMap(const Filename &filename);
	void CloseMap();
	void SetChanged();
	bool HasChanged();
	std::string GetFilename();
	void AddResource();
	void RemoveUnusedResources();
	bool RemoveResource(PQResourcePtr &res);
	void SelectAll();
	int DeselectAll();
	int DeleteSelection();
	void MoveSelection(float offset_x, float offset_y);

	void SelectByClassID(const string &classID);
	void SelectByName(const string &classID);

	void ScaleSelectionUp();
	void ScaleSelectionDown();

	void ConnectSelectedNodes();
	void BreakAllConnections();
	void BreakSelectedPairs();
	void ToggleDestination();

	bool SelBringForward();
	void SelBringToFront();
	bool SelSendBack();
	void SelSendToBack();
	
	PQResourcePtr GetCurrentBrush();
	void SetCurrentBrush(PQResourcePtr &brush);
	void ClearCurrentBrush();
	void SetCurrentTool(int tool);
	int GetCurrentTool();
	void SetCurrentLayer(int layer);
	int GetCurrentLayer();
	bool MouseDown(int x, int y);
	bool MouseUp(int x, int y, bool createItem = false);
	bool MouseMove(int x, int y);
	bool IsDragging();
	void SetGridSize(int size);
	int GetGridSize();
	void SetSnapDistance(int dist);
	int GetSnapDistance();
	void ShowGrid(bool show = true);
	bool IsGridVisible();
	void SnapToGrid(bool snap = true);
	bool IsSnappingToGrid();
	void AltKey(bool isDown = false);
	void ControlKey(bool isDown = false);
	void ShiftKey(bool isDown = false);
	bool IsAltDown();
	bool IsControlDown();
	bool IsShiftDown();
	bool IsScrolling();
	void SetScale(float scale);
	float GetScale();
	void SetViewport(int w, int h);
	void ResetView();
	void Zoom(int x, int y, float delta);
	int Draw(GL_Screen *dest);
	void ShowStats();
	Uint8 GetVersion();
	
	void Undo();
	void Redo();

	History mapHistory;
	
	PQMap *map;

	PQResourcePtr currentBrush;

private:

	void SnapToGrid(float &x, float &y, float w, float h);
	string StringCommas(long number);
	void Initialize();

	// the currently selected node from each layer
	PQMapNodePtr newNode;
	PQMapNodePtr selectionCenter;
	
	Ruler *selectedRuler;

	bool mouseDown;
	bool moving;
	bool controlDown;
	bool altDown;
	bool shiftDown;
	bool panning;

	float rel_drag_start_x;
	float rel_drag_start_y;
	float abs_drag_start_x;
	float abs_drag_start_y;

	float offset_x;
	float offset_y;

	float new_x;
	float new_y;

	Rect<float> rcDrag;

	// these should be synchronized for all instances of the editor
	Mat4<float> projection;
	Rect<float> viewPort;
	Rect<float> viewRect;
	
	float scale;
	float rcp_scale; // reciprocal of scale

	int currentTool;

	bool showGrid;
	bool snapToGrid;
	int gridSize;
	int snapDist;
	
};

extern PQMapEditor MapEditor;