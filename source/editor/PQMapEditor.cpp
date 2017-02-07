/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQMapEditor.h"

PQMapEditor MapEditor;

PQMapEditor::PQMapEditor()
{
	projection.SetIdentity();
	scale = 1.0f;
	rcp_scale = 1.0f;
	currentTool = TOOL_SELECTION;
	
	showGrid = true;
	snapToGrid = true;
	gridSize = 128;
	snapDist = 15;
	
	mapHistory.SetHistoryLevels(100);

	map = new PQMap;

	Initialize();
}

PQMapEditor::~PQMapEditor()
{
	delete map;
}

void PQMapEditor::Initialize()
{
	mapHistory.Clear();

	map->Initialize();

	mouseDown = false;
	moving = false;
	controlDown = false;
	altDown = false;
	panning = false;

	rel_drag_start_x = 0.0f;
	rel_drag_start_y = 0.0f;
	abs_drag_start_x = 0.0f;
	abs_drag_start_y = 0.0f;

	// RESET MAP POSITION TO ORIGIN
	rcDrag.Set(0, 0, 0, 0);
	
	newNode.reset();

	ResetView();

	selectedRuler = NULL;
	map->vRuler[1].x = 1024;
	map->hRuler[1].y = 1024;
}

void PQMapEditor::SetScale(float scale)
{
	this->scale = scale;
	this->rcp_scale = 1.0f / scale;
	
	viewRect.w = viewPort.w * rcp_scale;
	viewRect.h = viewPort.h * rcp_scale;
}

float PQMapEditor::GetScale()
{
	return scale;
}

void PQMapEditor::SetViewport(int w, int h)
{
	viewPort.w = (float)w;
	viewPort.h = (float)h;
	viewRect.w = viewPort.w * rcp_scale;
	viewRect.h = viewPort.h * rcp_scale;
}

void PQMapEditor::ResetView()
{
	viewRect.x = 0;
	viewRect.y = 0;
	SetScale(1.0f);
}

void PQMapEditor::Zoom(int x, int y, float delta)
{
	const float xf = (float)x;
	const float yf = (float)y;

	const float old_x = xf * rcp_scale;
	const float old_y = yf * rcp_scale;

	scale += delta * scale;
	scale = max(scale, MIN_ZOOM);
	scale = min(scale, MAX_ZOOM);

	rcp_scale = 1.0f / scale;

	const float new_x = xf * rcp_scale;
	const float new_y = yf * rcp_scale;

	viewRect.x += old_x - new_x;
	viewRect.y += old_y - new_y;
	viewRect.w = viewPort.w * rcp_scale;
	viewRect.h = viewPort.h * rcp_scale;
}

int PQMapEditor::Draw(GL_Screen *dest)
{
	int layer = map->GetCurrentLayer();

	// SET UP PROJECTION MATRIX FOR THE CURRENT VIEWPORT
	const float left = viewRect.x;
	const float right = viewRect.x + viewRect.w;
	const float bottom = viewRect.y + viewRect.h;
	const float top = viewRect.y;

	projection.SetOrtho2D(left,	right, bottom, top, 0.0f, 1.0f);
	
	dest->SetProjection(&projection);

// DRAW THE MAP
	map->Draw(dest, viewRect);

// DRAW NEW NODE BEING CREATED
	if(mouseDown == true && newNode )
	{
		if(newNode->rect.Intersects(viewRect))
			newNode->Draw(dest);
	}

// DRAW THE GRID
	if(showGrid == true)
	{
		float right = viewRect.x + viewRect.w;
		float bottom = viewRect.y + viewRect.h;

		for(float x = viewRect.x; x < right; x++)
		{
			int ix = (int)x;
			if(ix == 0)
				GL_DrawLine(dest, Vec2<float>(x, viewRect.y), Vec2<float>(x, bottom), Color(0, 0, 0), 3.0f);
			else if(ix % gridSize == 0)
				GL_DrawLine(dest, Vec2<float>(x, viewRect.y), Vec2<float>(x, bottom), Color(90, 90, 90));
		}
		
		for(float y = viewRect.y; y < bottom; y++)
		{
			int iy = (int)y;
			if(iy == 0)
				GL_DrawLine(dest, Vec2<float>(viewRect.x, y), Vec2<float>(right, y), Color(0, 0, 0), 3.0f);
			else if(iy % gridSize == 0)
				GL_DrawLine(dest, Vec2<float>(viewRect.x, y), Vec2<float>(right, y), Color(90, 90, 90));
		}
	}

// DRAW RULERS

	map->DrawRulers(dest, viewRect);

// DRAW SELECTED ITEMS
	switch(layer)
	{
		case LAYER_SPRITES:
		case LAYER_PEDGRAPH:
		case LAYER_VEHGRAPH:
		{
		// DRAW SELECTION RECTANGLES
			Rect<float> rcSel;
			
			SelectionSet &selection = map->GetSelection();

			MNodeIter it = selection.begin();
			while(it != selection.end())
			{
				PQMapNode &node = *(*it++);
				rcSel = node.rect;
		
				if(node.rect.Intersects(viewRect))
				{
					GL_DrawRect(dest, &rcSel, Color(255, 255, 255), 1);
				}

				if(moving == true)
				{
					float offset_x = new_x - selectionCenter->position.x;
					float offset_y = new_y - selectionCenter->position.y;

					rcSel.Offset(offset_x, offset_y);
			
					if(rcSel.Intersects(viewRect))
					{
						node.Draw(dest,	offset_x, offset_y,	1.0f, 0.5f);
						GL_DrawRect(dest, &rcSel, Color(255, 255, 255, 127), 1);
					}
				}
			}

		// DRAW THE SELECTION MARQUEE
			if(rcDrag.w > 0 && rcDrag.h > 0)
			{
				GL_DrawRect(dest, &rcDrag, Color(255, 255, 255), 1);
			}
		}

		break;

	case LAYER_RULERS:
		if(selectedRuler)
		{
			const float offset_x = new_x - selectedRuler->x;
			const float offset_y = new_y - selectedRuler->y;
			selectedRuler->Draw(dest, viewRect, true, offset_x, offset_y);
		}
		break;
	}
	return 0;
}

void PQMapEditor::Undo()
{
	mapHistory.Undo();
}

void PQMapEditor::Redo()
{
	mapHistory.Redo();
}

void PQMapEditor::SetChanged()
{
	map->SetChanged();
}

bool PQMapEditor::HasChanged()
{
	return map->HasChanged();
}

std::string PQMapEditor::GetFilename()
{
	return map->GetFilename();
}

void PQMapEditor::CloseMap()
{
	map->CloseMap();
	Initialize();
}

int PQMapEditor::LoadMap(const Filename &filename)
{
	Initialize();
	return map->LoadMap(filename);
}

int PQMapEditor::SaveMap(const Filename &filename)
{
	return map->SaveMap(filename);
	Initialize();
}

void PQMapEditor::AddResource()
{
	ResourceDialog rc(AfxGetApp()->GetMainWnd());
	PQResourcePtr res = rc.ShowDialog(&map->resources);
	
	if( res )
	{
		mapHistory.BeginSnapshot();
		{
			mapHistory.AddObjectState(&map->resources);
		}
		if(map->AddResource(res))
			mapHistory.EndSnapshot();
		else
			mapHistory.CancelSnapshot();
	}
}

void PQMapEditor::RemoveUnusedResources()
{
	mapHistory.BeginSnapshot();
	{
		mapHistory.AddObjectState(&map->resources);
	}
	if(map->RemoveUnusedResources())
		mapHistory.EndSnapshot();
	else
		mapHistory.CancelSnapshot();
}

bool PQMapEditor::RemoveResource(PQResourcePtr &res)
{
	// clear the current brush if it's showing this resource
	if(currentBrush == res)
		currentBrush.reset();
	
	int layer = map->GetCurrentLayer();

	mapHistory.BeginSnapshot();
		mapHistory.AddObjectState(&map->resources);
		
		if(layer == LAYER_SPRITES)
		{
			mapHistory.AddObjectState(&map->scene);
			mapHistory.AddObjectState(&map->sceneSelection);
			for(MNodeIter it = map->scene.begin(); it != map->scene.end(); it++)
				mapHistory.AddObjectState( (*it).get() );
		}

	// remove the resource from the map
	if( map->RemoveResource(res) )
		mapHistory.EndSnapshot();
	else
		mapHistory.CancelSnapshot();

	return true;
}

PQResourcePtr PQMapEditor::GetCurrentBrush()
{
	return currentBrush;
}

void PQMapEditor::SetCurrentBrush(PQResourcePtr &brush)
{
	currentBrush = brush;
}

void PQMapEditor::ClearCurrentBrush()
{
	currentBrush.reset();
}

void PQMapEditor::SetCurrentTool(int tool)
{
	currentTool = tool;
}

int PQMapEditor::GetCurrentTool()
{
	return currentTool;
}

void PQMapEditor::SetCurrentLayer(int layer)
{
	map->SetCurrentLayer(layer);
}

int PQMapEditor::GetCurrentLayer()
{
	return map->GetCurrentLayer();
}

bool PQMapEditor::MouseDown(int x, int y)
{
	bool ret = false;

	float world_x = viewRect.x + (x * viewRect.w / viewPort.w);
	float world_y = viewRect.y + (y * viewRect.h / viewPort.h);

	rel_drag_start_x = (float)x;
	rel_drag_start_y = (float)y;
	abs_drag_start_x = world_x;
	abs_drag_start_y = world_y;

	mouseDown = true;

	int layer = map->GetCurrentLayer();

	if(altDown == true)
	{
		panning = true;
	}
	else
	{
		switch(currentTool)
		{
		case TOOL_CREATION:
			///////////////////////
			switch(layer)
			{
			case LAYER_SPRITES:
				if( currentBrush )
				{
					switch(currentBrush->type)
					{
						case N_RES_IMAGE:
							newNode.reset( new PQMapImage(currentBrush) );
							break;

						case N_RES_SOUND:
							newNode.reset( new PQMapSound(currentBrush) );
							break;
					}

					float X = world_x;
					float Y = world_y;

					if(snapToGrid == true)
						SnapToGrid(X, Y, newNode->rect.w, newNode->rect.h);

					newNode->SetPosition(X, Y);
				}
				break;

			case LAYER_PEDGRAPH:
				newNode.reset( new PQPedGraphNode(world_x, world_y) );
				break;

			case LAYER_VEHGRAPH:
				newNode.reset( new PQVehGraphNode(world_x, world_y) );
				break;

			case LAYER_RULERS:
				// do nothing
				break;
			}
			//////////////////////////
			break;

		case TOOL_SELECTION:
			switch(layer)
			{
			case LAYER_SPRITES:
			case LAYER_PEDGRAPH:
			case LAYER_VEHGRAPH:
				selectionCenter = map->NodeFromPoint(world_x, world_y);

				// if a node was clicked
				if( selectionCenter )
				{
					if(selectionCenter->selected == false)
					{
						SelectionSet &selection = map->GetSelection();
					
						mapHistory.BeginSnapshot();
							mapHistory.AddObjectState( selectionCenter.get() );
							mapHistory.AddObjectState( &selection );
							for(MNodeIter it = selection.begin(); it != selection.end(); it++)
								mapHistory.AddObjectState( (*it).get() );
						mapHistory.EndSnapshot();

						if(controlDown == false)
							map->DeselectAll();

						map->SelectNode( selectionCenter );
					}
					
					new_x = selectionCenter->position.x;
					new_y = selectionCenter->position.y;

					moving = true;
				}
				break;

			case LAYER_RULERS:
				
				// check if one of the rulers was clicked
				if( map->vRuler[0].HitTest(world_x, world_y) )
				{
					selectedRuler = &map->vRuler[0];
				}
				else if( map->vRuler[1].HitTest(world_x, world_y) )
				{
					selectedRuler = &map->vRuler[1];
				}
				else if( map->hRuler[0].HitTest(world_x, world_y) )
				{
					selectedRuler = &map->hRuler[0];
				}
				else if( map->hRuler[1].HitTest(world_x, world_y) )
				{
					selectedRuler = &map->hRuler[1];
				}

				if(selectedRuler)
				{
					selectedRuler->selected = true;
					new_x = selectedRuler->x;
					new_y = selectedRuler->y;
					moving = true;
				}

				break;
			}
			break;
		}
	}

	return ret;
}

bool PQMapEditor::MouseMove(int x, int y)
{
	float world_x = viewRect.x + ((float)x * viewRect.w / viewPort.w);
	float world_y = viewRect.y + ((float)y * viewRect.h / viewPort.h);

	int layer = map->GetCurrentLayer();

	if(mouseDown == false)
		return false;

	if(panning == true)
	{
		viewRect.x -= ((float)x - rel_drag_start_x) * rcp_scale;
		viewRect.y -= ((float)y - rel_drag_start_y) * rcp_scale;
			
		rel_drag_start_x = (float)x;
		rel_drag_start_y = (float)y;
	}
	else
	{
		switch(currentTool)
		{
			case TOOL_CREATION:
			{
				if( newNode )
				{
					float X = world_x;
					float Y = world_y;

					if(snapToGrid == true)
						SnapToGrid(X, Y, newNode->rect.w, newNode->rect.h);

					newNode->SetPosition(X, Y);
				}
				break;
			}
			case TOOL_SELECTION:
			{
				switch(layer)
				{
				case LAYER_SPRITES:
				case LAYER_PEDGRAPH:
				case LAYER_VEHGRAPH:
					if(moving == true) // node was clicked
					{
						offset_x = world_x - abs_drag_start_x;
						offset_y = world_y - abs_drag_start_y;

						new_x = selectionCenter->position.x + offset_x;
						new_y = selectionCenter->position.y + offset_y;

						if(snapToGrid)
							SnapToGrid(new_x, new_y, selectionCenter->rect.w, selectionCenter->rect.h);
					}
					else // empty space was clicked
					{
						//update drag rect
						if(world_x < abs_drag_start_x)
						{
							rcDrag.x = world_x;
							rcDrag.w = abs_drag_start_x - world_x;
						}
						else
						{
							rcDrag.x = abs_drag_start_x;
							rcDrag.w = world_x - abs_drag_start_x;
						}

						if(world_y < abs_drag_start_y)
						{
							rcDrag.y = world_y;
							rcDrag.h = abs_drag_start_y - world_y;
						}
						else
						{
							rcDrag.y = abs_drag_start_y;
							rcDrag.h = world_y - abs_drag_start_y;
						}
					}
					break;
				case LAYER_RULERS:
					if(moving == true)
					{
						offset_x = world_x - abs_drag_start_x;
						offset_y = world_y - abs_drag_start_y;
						
						new_x = selectedRuler->x + offset_x;
						new_y = selectedRuler->y + offset_y;

						if(snapToGrid)
						{
							switch(selectedRuler->type)
							{
							case RULER_HORIZONTAL:
								SnapToGrid(new_x, new_y, -1, 1);
								break;
							case RULER_VERTICAL:
								SnapToGrid(new_x, new_y, 1, -1);
								break;
							}
						}
					}

					break;
				}
				break;
			}
		}
	}

	return true;
}

bool PQMapEditor::MouseUp(int x, int y, bool createItem)
{
	if(mouseDown == false)
		return false;

	mouseDown = false;

	if(panning == true)
	{
		panning = false;
		return false;
	}

	float world_x = viewRect.x + ((float)x * viewRect.w / viewPort.w);
	float world_y = viewRect.y + ((float)y * viewRect.h / viewPort.h);
	
	bool ret = false;
	int layer = map->GetCurrentLayer();

	switch(currentTool)
	{
	case TOOL_CREATION:
		if( newNode && createItem == true)
		{
			mapHistory.BeginSnapshot();

				int layer = map->GetCurrentLayer();
				if(layer == LAYER_SPRITES)
					mapHistory.AddObjectState(&map->scene);
				else if(layer == LAYER_PEDGRAPH)
					mapHistory.AddObjectState(&map->pedGraph);
				else if(layer == LAYER_VEHGRAPH)
					mapHistory.AddObjectState(&map->vehGraph);
			
			mapHistory.EndSnapshot();

			map->AddNode(newNode);
		}
		
		newNode.reset();

		break;

	case TOOL_SELECTION:
		if(createItem == true)
		{
			if(moving)
			{
				switch(layer)
				{
				case LAYER_SPRITES:
				case LAYER_PEDGRAPH:
				case LAYER_VEHGRAPH:
					//if not moved
					if( (fabs(world_x - abs_drag_start_x) < 2)
					&&  (fabs(world_y - abs_drag_start_y) < 2) )
					{
						bool changed = false;

						mapHistory.BeginSnapshot();

						if(controlDown == false)
						{
							SelectionSet &selection = map->GetSelection();

							if(selection.size() > 1)
							{
								mapHistory.AddObjectState(&selection);
								for(MNodeIter it = selection.begin(); it != selection.end(); it++)
									mapHistory.AddObjectState( (*it).get() );

								if(map->DeselectAll() > 0)
									changed = true;

								if(map->SelectNode(selectionCenter))
									changed = true;
							}
						}

						if(changed)
							mapHistory.EndSnapshot();
						else
							mapHistory.CancelSnapshot();
					}
					else // selection was moved
					{
						switch(layer)
						{
						case LAYER_SPRITES:
							mapHistory.BeginSnapshot(map->scene.size() + 2);
								mapHistory.AddObjectState(&map->sceneSelection);
								mapHistory.AddObjectState(&map->scene);
								for(MNodeIter it = map->scene.begin(); it != map->scene.end(); it++)
									mapHistory.AddObjectState( (*it).get() );
							mapHistory.EndSnapshot();
							break;
						case LAYER_PEDGRAPH:
							mapHistory.BeginSnapshot(map->pedGraph.size() + 2);
								mapHistory.AddObjectState(&map->pedGraphSelection);
								mapHistory.AddObjectState(&map->pedGraph);
								for(GNodeIter it = map->pedGraph.begin(); it != map->pedGraph.end(); it++)
									mapHistory.AddObjectState( (*it).get() );
							mapHistory.EndSnapshot();
							break;
						case LAYER_VEHGRAPH:
							mapHistory.BeginSnapshot(map->vehGraph.size() + 2);
								mapHistory.AddObjectState(&map->vehGraphSelection);
								mapHistory.AddObjectState(&map->vehGraph);
								for(GNodeIter it = map->vehGraph.begin(); it != map->vehGraph.end(); it++)
									mapHistory.AddObjectState( (*it).get() );
							mapHistory.EndSnapshot();
							break;

						}

						// Copying
						if(shiftDown == true)
						{
							SelectionSet &currentSelection = map->GetSelection();
							SelectionSet newSelection;
							
							MNodeIter it;
							for(it = currentSelection.begin(); it != currentSelection.end(); it++)
							{
								// copy the selected node
								PQMapNodePtr nodeCopy = (*it)->Copy();
							
								// add the new node to the map
								map->AddNode( nodeCopy );

								// add the new node to the selection set
								newSelection.push_back( nodeCopy );

								// deselect old node
								(*it)->selected = false;
							}

							// SWAP in the new selection set
							currentSelection = newSelection;
						}
					
						// move the selection set into place
						map->MoveSelection(new_x - selectionCenter->position.x, new_y - selectionCenter->position.y);
					}

					break;

				case LAYER_RULERS:
					if(selectedRuler)
					{
						selectedRuler->selected = false;

						if( (fabs(world_x - abs_drag_start_x) > 3)
						||  (fabs(world_y - abs_drag_start_y) > 3) )
						{
							mapHistory.BeginSnapshot();
								mapHistory.AddObjectState(selectedRuler);
							mapHistory.EndSnapshot();

							selectedRuler->MoveTo(new_x, new_y);
						}
					}

					break;
				}
			}
			else // if making selection rectangle
			{
				switch(layer)
				{
					case LAYER_SPRITES:
					case LAYER_PEDGRAPH:
					case LAYER_VEHGRAPH:
					{
						bool changed = false;
				
						SelectionSet &selection = map->GetSelection();

						mapHistory.BeginSnapshot();
							mapHistory.AddObjectState(&selection);

							if(layer == LAYER_SPRITES)
							{
								mapHistory.AddObjectState(&map->scene);
								for(MNodeIter it = map->scene.begin(); it != map->scene.end(); it++)
									mapHistory.AddObjectState( (*it).get() );
							}
							else if(layer == LAYER_PEDGRAPH)
							{
								mapHistory.AddObjectState(&map->pedGraph);
								for(GNodeIter it = map->pedGraph.begin(); it != map->pedGraph.end(); it++)
									mapHistory.AddObjectState( (*it).get() );
							}
							else if(layer == LAYER_VEHGRAPH)
							{
								mapHistory.AddObjectState(&map->vehGraph);
								for(GNodeIter it = map->vehGraph.begin(); it != map->vehGraph.end(); it++)
									mapHistory.AddObjectState( (*it).get() );
							}

						if(controlDown == false)
						{
							if(map->DeselectAll() > 0)
								changed = true;
						}

						if(rcDrag.w > 0 && rcDrag.h > 0)
						{
							int nSelected = map->SelectRect(rcDrag);
							if(nSelected > 0)
								changed = true;
						}

						if(changed)
							mapHistory.EndSnapshot();
						else
							mapHistory.CancelSnapshot();
						break;
					}
					case LAYER_RULERS:
					{
						// selection rect does not affect rulers
						break;
					}
				}
			}
		}

		break;
	}

	selectedRuler = NULL;
	rcDrag.Set(0, 0, 0, 0);
	selectionCenter.reset();
	moving = false;

	return ret;
}

bool PQMapEditor::IsDragging()
{
	return mouseDown;
}

void PQMapEditor::SnapToGrid(float &x, float &y, float w, float h)
{
	int halfGrid = gridSize >> 1;

///// SNAP X
	if(w > 0)
	{
		int ix = (int)(x + 0.5f);
		int iw = (int)(w + 0.5f);

		// will the user ever really venture out far enough into cyberspace for this to break?
		// not likely.
		Uint32 X  = (Uint32)(ix + 0x7FFFFFFF) - (Uint32)(iw >> 1);
		Uint32 X2 = (Uint32)(ix + 0x7FFFFFFF) + (Uint32)(iw >> 1);

		int remX  = (int)(X % gridSize);
		int remX2 = (int)(X2 % gridSize);
		bool XSnapLeft = true;
		bool X2SnapLeft = true;
		
		if(remX > halfGrid)
		{
			remX = gridSize - remX;
			XSnapLeft = false;
		}

		if(remX2 > halfGrid)
		{
			remX2 = gridSize - remX2;
			X2SnapLeft = false;
		}
	
		if(remX <= snapDist	&& remX <= remX2)
			x = (float)( (XSnapLeft == true) ? (ix - remX) : (ix + remX) );
		else if(remX2 <= snapDist && remX2 < remX)
			x = (float)( (X2SnapLeft == true) ? (ix - remX2) : (ix + remX2) );
	}

///// SNAP Y
	if(h > 0)
	{
		int iy = (int)(y + 0.5f);
		int ih = (int)(h + 0.5f);

		Uint32 Y  = (Uint32)(iy + 0x7FFFFFFF) - (Uint32)(ih >> 1);
		Uint32 Y2 = (Uint32)(iy + 0x7FFFFFFF) + (Uint32)(ih >> 1);

		int remY  = (int)(Y % gridSize);
		int remY2 = (int)(Y2 % gridSize);
		bool YSnapTop = true;
		bool Y2SnapTop = true;

		if(remY > halfGrid)
		{
			remY = gridSize - remY;
			YSnapTop = false;
		}

		if(remY2 > halfGrid)
		{
			remY2 = gridSize - remY2;
			Y2SnapTop = false;
		}

		if(remY <= snapDist	&& remY <= remY2)
			y = (float)( (YSnapTop == true) ? (iy - remY) : (iy + remY) );
		else if(remY2 <= snapDist && remY2 < remY)
			y = (float)( (Y2SnapTop == true) ? (iy - remY2) : (iy + remY2) );
	}
}

void PQMapEditor::SetGridSize(int size)
{
	if(size > MIN_GRID_SIZE && size < MAX_GRID_SIZE)
	{
		gridSize = size;
		
		if(snapDist > (gridSize >> 1))
			snapDist = (gridSize >> 1);
	}
}

int PQMapEditor::GetGridSize()
{
	return gridSize;
}

void PQMapEditor::SetSnapDistance(int dist)
{
	if(dist > 0 && dist < (gridSize >> 1))
	{
		snapDist = dist;
	}
}

int PQMapEditor::GetSnapDistance()
{
	return snapDist;
}

void PQMapEditor::ShowGrid(bool show)
{
	showGrid = show;
}

bool PQMapEditor::IsGridVisible()
{
	return showGrid;
}

void PQMapEditor::SnapToGrid(bool snap)
{
	snapToGrid = snap;
}

bool PQMapEditor::IsSnappingToGrid()
{
	return snapToGrid;
}

void PQMapEditor::ControlKey(bool isDown)
{
	if(isDown == true && mouseDown == false)
		controlDown = true;
	else if(isDown == false)
		controlDown = false;
}

void PQMapEditor::AltKey(bool isDown)
{
	if(isDown == true && mouseDown == false)
		altDown = true;
	else if(isDown == false)
		altDown = false;
}

void PQMapEditor::ShiftKey(bool isDown)
{
	if(isDown == true && mouseDown == false)
		shiftDown = true;
	else if(isDown == false)
		shiftDown = false;
}

bool PQMapEditor::IsAltDown()
{
	return altDown;
}

bool PQMapEditor::IsControlDown()
{
	return controlDown;
}

bool PQMapEditor::IsShiftDown()
{
	return shiftDown;
}

bool PQMapEditor::IsScrolling()
{
	return panning;
}

int PQMapEditor::DeleteSelection()
{
	int layer = map->GetCurrentLayer();

	switch(layer)
	{
	case LAYER_SPRITES:
		if(map->sceneSelection.size() > 0)
		{
			mapHistory.BeginSnapshot(map->sceneSelection.size() + 2);
				mapHistory.AddObjectState(&map->scene);
				mapHistory.AddObjectState(&map->sceneSelection);
				for(MNodeIter it = map->sceneSelection.begin(); it != map->sceneSelection.end(); it++)
					mapHistory.AddObjectState( (*it).get() );
			mapHistory.EndSnapshot();
		}
		break;

	case LAYER_PEDGRAPH:
		if(map->pedGraphSelection.size() > 0)
		{
			mapHistory.BeginSnapshot(map->pedGraphSelection.size() + 2);
				mapHistory.AddObjectState(&map->pedGraph);
				mapHistory.AddObjectState(&map->pedGraphSelection);
				for(MNodeIter it = map->pedGraphSelection.begin(); it != map->pedGraphSelection.end(); it++)
					mapHistory.AddObjectState( (*it).get() );
			mapHistory.EndSnapshot();
		}
		break;

	case LAYER_VEHGRAPH:
		if(map->vehGraphSelection.size() > 0)
		{
			mapHistory.BeginSnapshot(map->vehGraphSelection.size() + 2);
				mapHistory.AddObjectState(&map->vehGraph);
				mapHistory.AddObjectState(&map->vehGraphSelection);
				for(MNodeIter it = map->vehGraphSelection.begin(); it != map->vehGraphSelection.end(); it++)
					mapHistory.AddObjectState( (*it).get() );
			mapHistory.EndSnapshot();
		}
		break;

	case LAYER_RULERS:

		break;
	}

	return map->DeleteSelection();
}

bool PQMapEditor::SelBringForward()
{
	int layer = map->GetCurrentLayer();

	switch(layer)
	{
	case LAYER_SPRITES:
		mapHistory.BeginSnapshot();	
			mapHistory.AddObjectState(&map->scene);
			if(map->SelBringForward())
			{
				mapHistory.EndSnapshot();
				return true;
			}
		mapHistory.CancelSnapshot();

		break;
	case LAYER_PEDGRAPH:
		break;

	case LAYER_VEHGRAPH:
		break;

	case LAYER_RULERS:
		break;

	}
	
	return false;
}

void PQMapEditor::SelBringToFront()
{
	int layer = map->GetCurrentLayer();

	switch(layer)
	{
	case LAYER_SPRITES:
		mapHistory.BeginSnapshot();
			mapHistory.AddObjectState(&map->scene);
			
			if(map->SelBringToFront() > 0)
			{
				mapHistory.EndSnapshot();
				break;
			}

		mapHistory.CancelSnapshot();

		break;
	case LAYER_PEDGRAPH:
		break;

	case LAYER_VEHGRAPH:
		break;

	case LAYER_RULERS:
		break;

	}
}

bool PQMapEditor::SelSendBack()
{
	int layer = map->GetCurrentLayer();
	
	switch(layer)
	{
	case LAYER_SPRITES:
		mapHistory.BeginSnapshot();
			mapHistory.AddObjectState(&map->scene);
			
			if(map->SelSendBack())
			{
				mapHistory.EndSnapshot();
				return true;
			}

		mapHistory.CancelSnapshot();

		break;
	case LAYER_PEDGRAPH:
		break;

	case LAYER_VEHGRAPH:
		break;

	case LAYER_RULERS:
		break;

	}

	return false;
}

void PQMapEditor::SelSendToBack()
{
	int layer = map->GetCurrentLayer();
	switch(layer)
	{
	case LAYER_SPRITES:
		mapHistory.BeginSnapshot();
			mapHistory.AddObjectState(&map->scene);
			
			if(map->SelSendToBack() > 0)
			{
				mapHistory.EndSnapshot();
				break;
			}

		mapHistory.CancelSnapshot();

		break;
	case LAYER_PEDGRAPH:
		break;

	case LAYER_VEHGRAPH:
		break;

	case LAYER_RULERS:
		break;

	}
}

void PQMapEditor::MoveSelection(float offset_x, float offset_y)
{
	map->MoveSelection(offset_x, offset_y);
}

void PQMapEditor::SelectByClassID(const string &classID)
{
	int layer = map->GetCurrentLayer();

	if(layer != LAYER_SPRITES)
		return;

	mapHistory.BeginSnapshot(map->scene.size() + 1);
		mapHistory.AddObjectState(&map->sceneSelection);
		for(MNodeIter it = map->scene.begin(); it != map->scene.end(); it++)
			mapHistory.AddObjectState( (*it).get() );
	
	bool changed = false;

	if(map->DeselectAll() > 0)
		changed = true;

	for(MNodeIter it = map->scene.begin(); it != map->scene.end(); it++)
	{
		PQMapNodePtr &ptr = (*it);
		if(classID == ptr->res->class_id)
		{
			map->SelectNode(ptr);
			changed = true;
		}
	}

	if(changed)
		mapHistory.EndSnapshot();
	else
		mapHistory.CancelSnapshot();
}

void PQMapEditor::SelectByName(const string &classID)
{
	int layer = map->GetCurrentLayer();
	if(layer != LAYER_SPRITES)
		return;

	mapHistory.BeginSnapshot(map->scene.size() + 1);
		mapHistory.AddObjectState(&map->sceneSelection);
		for(MNodeIter it = map->scene.begin(); it != map->scene.end(); it++)
			mapHistory.AddObjectState( (*it).get() );

	bool changed = false;

	if(map->DeselectAll() > 0)
		changed = true;

	for(MNodeIter it = map->scene.begin(); it != map->scene.end(); it++)
	{
		PQMapNodePtr &ptr = (*it);
		if(classID == ptr->res->resource_name)
		{
			map->SelectNode(ptr);
			changed = true;
		}
	}

	if(changed)
		mapHistory.EndSnapshot();
	else
		mapHistory.CancelSnapshot();
}

void PQMapEditor::ScaleSelectionUp()
{
	int layer = map->GetCurrentLayer();
	if(layer != LAYER_SPRITES)
		return;

	SelectionSet &selection = map->GetSelection();
	if(selection.size() == 0)
		return;

	mapHistory.BeginSnapshot(selection.size() + 1);
		mapHistory.AddObjectState(&selection);
		for(MNodeIter it = selection.begin(); it != selection.end(); it++)
			mapHistory.AddObjectState( (*it).get() );

	if( map->ScaleSelectionUp() )
		mapHistory.EndSnapshot();
	else
		mapHistory.CancelSnapshot();
}

void PQMapEditor::ScaleSelectionDown()
{
	int layer = map->GetCurrentLayer();
	if(layer != LAYER_SPRITES)
		return;

	SelectionSet &selection = map->GetSelection();
	if(selection.size() == 0)
		return;

	mapHistory.BeginSnapshot(selection.size() + 1);
		mapHistory.AddObjectState(&selection);
		for(MNodeIter it = selection.begin(); it != selection.end(); it++)
			mapHistory.AddObjectState( (*it).get() );

	if( map->ScaleSelectionDown() )
		mapHistory.EndSnapshot();
	else
		mapHistory.CancelSnapshot();
}

void PQMapEditor::SelectAll()
{
	int layer = map->GetCurrentLayer();

	switch(layer)
	{
	case LAYER_SPRITES:
		mapHistory.BeginSnapshot();
			mapHistory.AddObjectState(&map->sceneSelection);
			mapHistory.AddObjectState(&map->scene);
			for(MNodeIter it = map->scene.begin(); it != map->scene.end(); it++)
				mapHistory.AddObjectState( (*it).get() );

		if(map->SelectAll() > 0)
			mapHistory.EndSnapshot();
		else
			mapHistory.CancelSnapshot();

		break;

	case LAYER_PEDGRAPH:
		mapHistory.BeginSnapshot();
			mapHistory.AddObjectState(&map->pedGraphSelection);
			mapHistory.AddObjectState(&map->pedGraph);
			for(GNodeIter it = map->pedGraph.begin(); it != map->pedGraph.end(); it++)
				mapHistory.AddObjectState( (*it).get() );

		if(map->SelectAll() > 0)
			mapHistory.EndSnapshot();
		else
			mapHistory.CancelSnapshot();

		break;

	case LAYER_VEHGRAPH:
		mapHistory.BeginSnapshot();
			mapHistory.AddObjectState(&map->vehGraphSelection);
			mapHistory.AddObjectState(&map->vehGraph);
			for(GNodeIter it = map->vehGraph.begin(); it != map->vehGraph.end(); it++)
				mapHistory.AddObjectState( (*it).get() );

		if(map->SelectAll() > 0)
			mapHistory.EndSnapshot();
		else
			mapHistory.CancelSnapshot();

		break;

	case LAYER_RULERS:

		break;
	}
}

int PQMapEditor::DeselectAll()
{
	int layer = map->GetCurrentLayer();
	int nDeselected = 0;

	switch(layer)
	{
	case LAYER_SPRITES:
		mapHistory.BeginSnapshot(map->sceneSelection.size() + 1);
			mapHistory.AddObjectState(&map->sceneSelection);
			for(MNodeIter it = map->sceneSelection.begin(); it != map->sceneSelection.end(); it++)
				mapHistory.AddObjectState( (*it).get() );

			nDeselected = map->DeselectAll();

			if(nDeselected > 0)
				mapHistory.EndSnapshot();
			else
				mapHistory.CancelSnapshot();

		break;

	case LAYER_PEDGRAPH:
		mapHistory.BeginSnapshot(map->pedGraphSelection.size() + 1);
			mapHistory.AddObjectState(&map->pedGraphSelection);
			for(MNodeIter it = map->pedGraphSelection.begin(); it != map->pedGraphSelection.end(); it++)
				mapHistory.AddObjectState( (*it).get() );

			nDeselected = map->DeselectAll();

			if(nDeselected > 0)
				mapHistory.EndSnapshot();
			else
				mapHistory.CancelSnapshot();

		break;

	case LAYER_VEHGRAPH:
		mapHistory.BeginSnapshot(map->vehGraphSelection.size() + 1);
			mapHistory.AddObjectState(&map->vehGraphSelection);
			for(MNodeIter it = map->vehGraphSelection.begin(); it != map->vehGraphSelection.end(); it++)
				mapHistory.AddObjectState( (*it).get() );

			nDeselected = map->DeselectAll();

			if(nDeselected > 0)
				mapHistory.EndSnapshot();
			else
				mapHistory.CancelSnapshot();

		break;

	case LAYER_RULERS:

		break;
	}

	return nDeselected;
}

void PQMapEditor::ConnectSelectedNodes()
{
	int layer = map->GetCurrentLayer();
	
	switch(layer)
	{
		case LAYER_SPRITES:

			break;

		case LAYER_PEDGRAPH:
			mapHistory.BeginSnapshot(map->pedGraph.size() + 1);
			mapHistory.AddObjectState(&map->pedGraph);
			for(GNodeIter it = map->pedGraph.begin(); it != map->pedGraph.end(); it++)
				mapHistory.AddObjectState( (*it).get() );
			break;

		case LAYER_VEHGRAPH:
			mapHistory.BeginSnapshot(map->vehGraph.size() + 1);
			mapHistory.AddObjectState(&map->vehGraph);
			for(GNodeIter it = map->vehGraph.begin(); it != map->vehGraph.end(); it++)
				mapHistory.AddObjectState( (*it).get() );
			break;

		case LAYER_RULERS:

			break;
	}

	if(mapHistory.SnapshotOpen())
	{
		if( map->ConnectSelectedNodes() )
			mapHistory.EndSnapshot();
		else
			mapHistory.CancelSnapshot();
	}
}

void PQMapEditor::BreakAllConnections()
{
	int layer = map->GetCurrentLayer();

	switch(layer)
	{
		case LAYER_SPRITES:

			break;

		case LAYER_PEDGRAPH:
			mapHistory.BeginSnapshot(map->pedGraph.size() + 1);
			mapHistory.AddObjectState(&map->pedGraph);
			for(GNodeIter it = map->pedGraph.begin(); it != map->pedGraph.end(); it++)
				mapHistory.AddObjectState( (*it).get() );
			break;

		case LAYER_VEHGRAPH:
			mapHistory.BeginSnapshot(map->vehGraph.size() + 1);
			mapHistory.AddObjectState(&map->vehGraph);
			for(GNodeIter it = map->vehGraph.begin(); it != map->vehGraph.end(); it++)
				mapHistory.AddObjectState( (*it).get() );
			break;

		case LAYER_RULERS:

			break;
	}

	if(mapHistory.SnapshotOpen())
	{
		if( map->BreakAllConnections() )
			mapHistory.EndSnapshot();
		else
			mapHistory.CancelSnapshot();
	}
}

void PQMapEditor::BreakSelectedPairs()
{
	int layer = map->GetCurrentLayer();

	switch(layer)
	{
		case LAYER_SPRITES:

			break;

		case LAYER_PEDGRAPH:
			mapHistory.BeginSnapshot(map->pedGraphSelection.size() + 1);
			mapHistory.AddObjectState(&map->pedGraphSelection);
			for(MNodeIter it = map->pedGraphSelection.begin(); it != map->pedGraphSelection.end(); it++)
				mapHistory.AddObjectState( (*it).get() );
			break;

		case LAYER_VEHGRAPH:
			mapHistory.BeginSnapshot(map->vehGraphSelection.size() + 1);
			mapHistory.AddObjectState(&map->vehGraphSelection);
			for(MNodeIter it = map->vehGraphSelection.begin(); it != map->vehGraphSelection.end(); it++)
				mapHistory.AddObjectState( (*it).get() );
			break;

		case LAYER_RULERS:

			break;
	}

	if(mapHistory.SnapshotOpen())
	{
		if( map->BreakSelectedPairs() )
			mapHistory.EndSnapshot();
		else
			mapHistory.CancelSnapshot();
	}
}

void PQMapEditor::ToggleDestination()
{
	int layer = map->GetCurrentLayer();
	
	switch(layer)
	{
		case LAYER_SPRITES:

			break;

		case LAYER_PEDGRAPH:
			mapHistory.BeginSnapshot(map->pedGraphSelection.size() + 1);
			mapHistory.AddObjectState(&map->pedGraphSelection);
			for(MNodeIter it = map->pedGraphSelection.begin(); it != map->pedGraphSelection.end(); it++)
				mapHistory.AddObjectState( (*it).get() );
			break;

		case LAYER_VEHGRAPH:
			mapHistory.BeginSnapshot(map->vehGraphSelection.size() + 1);
			mapHistory.AddObjectState(&map->vehGraphSelection);
			for(MNodeIter it = map->vehGraphSelection.begin(); it != map->vehGraphSelection.end(); it++)
				mapHistory.AddObjectState( (*it).get() );
			break;

		case LAYER_RULERS:

			break;
	}

	if(mapHistory.SnapshotOpen())
	{
		if( map->ToggleDestination() )
			mapHistory.EndSnapshot();
		else
			mapHistory.CancelSnapshot();
	}
}

void PQMapEditor::ShowStats()
{
	Filename file;

	long mapHeaderUsed = 0;
	long fileMemUsed = 0;
	long resMemUsed = 0;
	long sceneMemUsed = 0;

	mapHeaderUsed = sizeof(Uint8) + sizeof(unsigned int) + sizeof(int); // version, time limit, number of deliveries

	PQResImagePtr pImg;
	PQResSoundPtr pSnd;

	for(Uint16 i = 0; i < map->resources.size(); i++)
	{
		switch(map->resources[i]->type)
		{
		case N_RES_IMAGE:
			pImg = dynamic_pointer_cast<PQResImage>(map->resources[i]);
			fileMemUsed += pImg->img->GetSheetWidth() * pImg->img->GetSheetHeight() * 4;
			break;

		case N_RES_SOUND:
			pSnd = dynamic_pointer_cast<PQResSound>(map->resources[i]);
			break;
		}

		resMemUsed += map->resources[i]->GetDataSize();
	}

	MNodeIter it = map->scene.begin();
	while(it != map->scene.end())
	{
		sceneMemUsed += (*it++)->GetDataSize();
	}

	long totalMemUsed = mapHeaderUsed + fileMemUsed + resMemUsed + sceneMemUsed;

	stringstream strStats;
	strStats << "Estimated memory usage for this map file:" << endl;
	strStats << "Map Header: " << StringCommas(mapHeaderUsed) << "bytes" << endl;
	strStats << "Source Files: " << StringCommas(fileMemUsed) << "bytes" << endl;
	strStats << "Resource Data: " << StringCommas(resMemUsed) << "bytes" << endl;
	strStats << "Scene Data: " << StringCommas(sceneMemUsed) << "bytes" << endl;
	strStats << "===========================" << endl;
	
	strStats << "Total Memory Used: " << StringCommas(totalMemUsed) << "bytes" << endl;
	
	MessageBox(0, strStats.str().c_str(), "Memory Usage Statistics", MB_OK | MB_ICONINFORMATION);
}

string PQMapEditor::StringCommas(long number)
{
	stringstream s;
	s << number;
	
	string str = s.str();
	string final;

	while(str.size() > 0)
	{
		if(str.size() > 3)
		{
			if(final == "")
				final = str.substr(str.size() - 3, 3);
			else
				final = str.substr(str.size() - 3, 3) + "," + final;
				
			str.erase(str.size() - 3, 3);
		}
		else
		{
			if(final == "")
				final = str;
			else
				final = str + "," + final;

			str = "";
		}
	}

	return final;
}

Uint8 PQMapEditor::GetVersion()
{
	return PQ_MAP_FILE_MAX_VERSION;
}