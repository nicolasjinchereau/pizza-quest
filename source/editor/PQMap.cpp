/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQMap.h"

Filename PQMap::filename;
bool PQMap::hasChanged = false;
int PQMap::currentLayer = LAYER_SPRITES;

PQMap::PQMap()
{
	resources.reserve(MAX_RESOURCES);
	currentLayer = LAYER_SPRITES;
	Initialize();
}

PQMap::~PQMap()
{

}

void PQMap::Initialize()
{
	filename = "";
	hasChanged = false;

	resources.clear();
	scene.clear();
	pedGraph.clear();
	vehGraph.clear();

	sceneSelection.clear();
	pedGraphSelection.clear();
	vehGraphSelection.clear();

	timeLimit = 60000;
	numDeliveries = 1;

	vRuler[0].x = 0;
	vRuler[1].x = 1024;

	hRuler[0].y = 0;
	hRuler[1].y = 1024;
}

void PQMap::SetCurrentLayer(int layer)
{
	currentLayer = layer;
}

int PQMap::GetCurrentLayer()
{
	return currentLayer;
}

std::string PQMap::GetFilename()
{
	return filename;
}

void PQMap::SetChanged()
{
	hasChanged = true;
}

bool PQMap::HasChanged()
{
	return hasChanged;
}

void PQMap::CloseMap()
{
	Initialize();
}

void PQMap::Draw(GL_Screen *dest, Rect<float> &viewRect)
{
	float sceneShade;
	float pedGraphShade;
	float vehGraphShade;
	float rulerShade;

	// DRAW ALL MAP NODES
	switch(currentLayer)
	{
	case LAYER_SPRITES:
		sceneShade = 1.0f;
		pedGraphShade = 0.7f;
		vehGraphShade = 0.7f;
		rulerShade = 0.7f;
		break;

	case LAYER_PEDGRAPH:
		sceneShade = 0.7f;
		pedGraphShade = 1.0f;
		vehGraphShade = 0.7f;
		rulerShade = 0.7f;
		break;

	case LAYER_VEHGRAPH:
		sceneShade = 0.7f;
		pedGraphShade = 0.7f;
		vehGraphShade = 1.0f;
		rulerShade = 0.7f;
		break;

	case LAYER_RULERS:
		sceneShade = 0.7f;
		pedGraphShade = 0.7f;
		vehGraphShade = 0.7f;
		rulerShade = 1.0f;
		break;
	}
	
	// DRAW SCENE NODES
	MNodeIter s = scene.begin();
	while(s != scene.end())
	{
		PQMapNode &node = *(*s);
		if(node.rect.Intersects(viewRect))
			node.Draw(dest, 0.0f, 0.0f, sceneShade);
		s++;
	}

	// DRAW PEDESTRIAN GRAPH
	PQPedGraphNodePtr pgNode;
	for(GNodeIter p = pedGraph.begin(); p != pedGraph.end(); p++)
	{
		pgNode = dynamic_pointer_cast<PQPedGraphNode>(*p);
		pgNode->TraceNeighbours(dest, viewRect);

		if(viewRect.PointInside(pgNode->position.x, pgNode->position.y))
			pgNode->Draw(dest, viewRect, pedGraphShade);
	}

	// DRAW VEHICLE GRAPH
	PQVehGraphNodePtr vgNode;
	for(GNodeIter v = vehGraph.begin(); v != vehGraph.end(); v++)
	{
		vgNode = dynamic_pointer_cast<PQVehGraphNode>(*v);
		vgNode->TraceNeighbours(dest, viewRect);

		if(viewRect.PointInside(vgNode->position.x, vgNode->position.y))
			vgNode->Draw(dest, viewRect, vehGraphShade);
	}
}

void PQMap::DrawRulers(GL_Screen *dest, const Rect<float> &viewRect)
{
	const bool active = currentLayer == LAYER_RULERS;

	vRuler[0].Draw(dest, viewRect, active);
	vRuler[1].Draw(dest, viewRect, active);
	hRuler[0].Draw(dest, viewRect, active);
	hRuler[1].Draw(dest, viewRect, active);
}

SelectionSet &PQMap::GetSelection()
{
	switch(currentLayer)
	{
	case LAYER_SPRITES:
		return sceneSelection;

	case LAYER_PEDGRAPH:
		return pedGraphSelection;

	case LAYER_VEHGRAPH:
		return vehGraphSelection;
		
	case LAYER_RULERS:
		
		break;
	}

	// return scene by default
	return sceneSelection;
}

//MapNodeList &PQMap::GetNodeList()
//{
//	switch(currentLayer)
//	{
//	case LAYER_SPRITES:
//		return scene;
//
//	case LAYER_PEDGRAPH:
//		return dynamic_pointer_cast<MapNodeList>(pedGraph);
//
//	case LAYER_VEHGRAPH:
//		return vehGraph;
//	}
//
//	// return scene by default
//	return sceneSelection;
//}

void PQMap::AddNode(PQMapNodePtr &node)
{
	switch(currentLayer)
	{
		case LAYER_SPRITES:
		{
			scene.push_back(node);
			break;
		}
		case LAYER_PEDGRAPH:
		{
			PQGraphNodePtr pedNode = dynamic_pointer_cast<PQGraphNode>(node);
			assert(pedNode);
			pedGraph.push_back( pedNode );
			break;
		}
		case LAYER_VEHGRAPH:
		{
			PQGraphNodePtr vehNode = dynamic_pointer_cast<PQGraphNode>(node);
			assert(vehNode);
			vehGraph.push_back( vehNode );
			break;
		}
		case LAYER_RULERS:
		{

		}
	}

	SetChanged();
}

void PQMap::RemoveNode(PQMapNodePtr &node)
{
	switch(currentLayer)
	{
		case LAYER_SPRITES:
		{
			scene.remove(node);
			break;
		}
		case LAYER_PEDGRAPH:
		{
			PQGraphNodePtr pedNode = dynamic_pointer_cast<PQGraphNode>(node);
			assert(pedNode);

			for(int i = 0; i < (int)pedGraph.size(); i++)
			{
				if(pedGraph[i] == pedNode)
				{
					pedGraph[i]->DisconnectAll();
					pedGraph.erase(pedGraph.begin() + i);
					break;
				}
			}

			break;
		}
		case LAYER_VEHGRAPH:
		{
			PQGraphNodePtr vehNode = dynamic_pointer_cast<PQGraphNode>(node);
			assert(vehNode);
			for(int i = 0; i < (int)vehGraph.size(); i++)
			{
				if(vehGraph[i] == vehNode)
				{
					vehGraph[i]->DisconnectAll();
					vehGraph.erase(vehGraph.begin() + i);
					break;
				}
			}
			break;
		}
	}

	SetChanged();
}

PQMapNodePtr PQMap::NodeFromPoint(float x, float y)
{
	switch(currentLayer)
	{
		case LAYER_SPRITES:
		{
			MNodeIter itScene = scene.rbegin();
			while(itScene != scene.rend())
			{
				if((*itScene)->rect.PointInside(x, y))
					return (*itScene);
				itScene--;
			}
			break;
		}
		case LAYER_PEDGRAPH:
		{
			GNodeIter itPed = pedGraph.begin();
			while(itPed != pedGraph.end())
			{
				if((*itPed)->rect.PointInside(x, y))
					return (*itPed);
				itPed++;
			}
			break;
		}
		case LAYER_VEHGRAPH:
		{
			GNodeIter itVeh = vehGraph.begin();
			while(itVeh != vehGraph.end())
			{
				if((*itVeh)->rect.PointInside(x, y))
					return (*itVeh);
				itVeh++;
			}
			break;
		}
	}

	return PQMapNodePtr();
}

bool PQMap::SelectNode(PQMapNodePtr &node)
{
	if(node->selected == false)
	{
		node->selected = true;
		SelectionSet &selection = GetSelection();
		selection.push_back(node);
		return true;
	}

	return false;
}

void PQMap::DeselectNode(PQMapNodePtr &node)
{
	if(node->selected == true)
	{
		node->selected = false;
		SelectionSet &selection = GetSelection();
		selection.remove(node);
	}
}

int PQMap::SelectAll()
{
	int nSelected = 0;

	switch(currentLayer)
	{
		case LAYER_SPRITES:
		{
			MNodeIter it = scene.begin();
			while(it != scene.end())
			{
				if(SelectNode((*it++)))
					nSelected++;
			}

			break;
		}
		case LAYER_PEDGRAPH:
		{
			GNodeIter itPed = pedGraph.begin();
			while(itPed != pedGraph.end())
			{
				if(SelectNode( dynamic_pointer_cast<PQMapNode>((*itPed++)) ))
					nSelected++;
			}

			break;
		}
		case LAYER_VEHGRAPH:
		{
			GNodeIter itVeh = vehGraph.begin();
			while(itVeh != vehGraph.end())
			{
				if(SelectNode( dynamic_pointer_cast<PQMapNode>((*itVeh++)) ))
					nSelected++;
			}

			break;
		}
	}

	return nSelected;
}

int PQMap::DeselectAll()
{
	int nUnselected = 0;

	SelectionSet &selection = GetSelection();
	
	MNodeIter it = selection.begin();
	while(it != selection.end())
	{
		if((*it)->selected == true)
		{
			(*it)->selected = false;
			nUnselected++;
		}
		it++;
	}

	selection.clear();

	return nUnselected;
}

int PQMap::SelectRect(Rect<float> &rect)
{
	int nSelected = 0;

	PQMapNodePtr pNode;
	
	switch(currentLayer)
	{
		case LAYER_SPRITES:
		{
			MNodeIter it = scene.begin();
			while(it != scene.end())
			{
				PQMapNodePtr &pNode = (*it);
				if(pNode->rect.Intersects(rect))
				{
					SelectNode(pNode);
					nSelected++;
				}
				it++;
			}

			break;
		}
		case LAYER_PEDGRAPH:
		{
			GNodeIter it = pedGraph.begin();
			while(it != pedGraph.end())
			{
				PQGraphNodePtr &pNode = (*it);
				if(pNode->rect.Intersects(rect))
				{
					SelectNode( dynamic_pointer_cast<PQMapNode>(pNode) );
					nSelected++;
				}
				it++;
			}

			break;
		}
		case LAYER_VEHGRAPH:
		{
			GNodeIter it = vehGraph.begin();
			while(it != vehGraph.end())
			{
				PQGraphNodePtr &pNode = (*it);
				if(pNode->rect.Intersects(rect))
				{
					SelectNode( dynamic_pointer_cast<PQMapNode>(pNode) );
					nSelected++;
				}
				it++;
			}

			break;
		}
	}

	return nSelected;
}

int PQMap::DeleteSelection()
{
	int nDeleted = 0;

	SelectionSet &selection = GetSelection();

	MNodeIter it = selection.begin();
	while(it != selection.end())
	{
		RemoveNode((*it++));
		nDeleted++;
	}

	selection.clear();
	
	if(nDeleted > 0)
		SetChanged();

	return nDeleted;
}

void PQMap::MoveSelection(float offset_x, float offset_y)
{
	SelectionSet &selection = GetSelection();

	MNodeIter it = selection.begin();
	while(it != selection.end())
	{
		(*it++)->Move(offset_x, offset_y);
	}

	if(selection.size() > 0)
		SetChanged();
}

bool PQMap::ScaleSelectionUp()
{
	SelectionSet &selection = GetSelection();

	MNodeIter it = sceneSelection.begin();
	while(it != sceneSelection.end())
	{
		if( (*it)->type == N_MAP_IMAGE)
		{
			PQMapImage *mImg = (PQMapImage*)(*it).get();
			mImg->scale += 0.05f;
			mImg->CalcRect();
		}

		(*it++);
	}

	if(selection.size() > 0)
	{
		SetChanged();
		return true;
	}

	return false;
}

bool PQMap::ScaleSelectionDown()
{
	SelectionSet &selection = GetSelection();

	MNodeIter it = sceneSelection.begin();
	while(it != sceneSelection.end())
	{
		if( (*it)->type == N_MAP_IMAGE)
		{
			PQMapImage *mImg = (PQMapImage*)(*it).get();
			mImg->scale -= 0.05f;
			mImg->CalcRect();
		}

		(*it++);
	}

	if(selection.size() > 0)
	{
		SetChanged();
		return true;
	}

	return false;
}

bool PQMap::ConnectSelectedNodes()
{
	if(currentLayer == LAYER_PEDGRAPH || currentLayer == LAYER_VEHGRAPH)
	{
		SelectionSet &selection = GetSelection();

		// some nodes will definately end up being connected
		// if there are 2 or more nodes selected
		if(selection.size() > 1)
		{
			PQGraphNodePtr gPtr1;
			PQGraphNodePtr gPtr2;

			MNodeIter it1 = selection.begin();
			MNodeIter it2;

			while(it1 != selection.rend())
			{
				gPtr1 = dynamic_pointer_cast<PQGraphNode>(*it1);

				it2 = it1;
				it2++;

				while(it2 != selection.end())
				{
					gPtr2 = dynamic_pointer_cast<PQGraphNode>(*it2);
				
					gPtr1->AddNeighbour(gPtr2);
					gPtr2->AddNeighbour(gPtr1);

					it2++;
				}
				it1++;
			}

			SetChanged();

			return true;
		}
	}

	return false;
}

bool PQMap::BreakAllConnections()
{
	if(currentLayer == LAYER_PEDGRAPH || currentLayer == LAYER_VEHGRAPH)
	{
		SelectionSet &selection = GetSelection();
		
		if(selection.size() > 0)
		{
			PQGraphNodePtr gPtr;

			MNodeIter it = selection.begin();

			while(it != selection.end())
			{
				gPtr = dynamic_pointer_cast<PQGraphNode>(*it);
				gPtr->DisconnectAll();
				it++;
			}
			
			SetChanged();

			return true;
		}
	}

	return false;
}

bool PQMap::BreakSelectedPairs()
{
	if(currentLayer == LAYER_PEDGRAPH || currentLayer == LAYER_VEHGRAPH)
	{
		SelectionSet &selection = GetSelection();
		
		if(selection.size() > 1)
		{
			PQGraphNodePtr gPtr;

			MNodeIter it = selection.begin();

			while(it != selection.end())
			{
				gPtr = dynamic_pointer_cast<PQGraphNode>(*it);
				gPtr->DisconnectSelected();
				it++;
			}

			SetChanged();

			return true;
		}
	}

	return false;
}

bool PQMap::ToggleDestination()
{
	if(currentLayer == LAYER_PEDGRAPH || currentLayer == LAYER_VEHGRAPH)
	{
		SelectionSet &selection = GetSelection();
		
		if(selection.size() > 0)
		{
			PQGraphNodePtr gPtr;
			unsigned int nDestinations = 0;

			MNodeIter it = selection.begin();
			while(it != selection.end())
			{
				gPtr = dynamic_pointer_cast<PQGraphNode>(*it);
				if(gPtr->isDestination == true)
					nDestinations++;
				it++;
			}

			if( nDestinations > (selection.size() >> 1) )
			{
				for(it = selection.begin(); it != selection.end(); it++)
				{
					gPtr = dynamic_pointer_cast<PQGraphNode>(*it);
					gPtr->isDestination = false;
				}
			}
			else
			{
				for(it = selection.begin(); it != selection.end(); it++)
				{
					gPtr = dynamic_pointer_cast<PQGraphNode>(*it);
					gPtr->isDestination = true;
				}
			}

			SetChanged();

			return true;
		}
	}

	return false;
}

bool PQMap::SelBringForward()
{
	if(currentLayer != LAYER_SPRITES)
		return false;

	bool ret = false;

	MNodeIter itLast;
	MNodeIter it = scene.rbegin(); //rbegin = front

	while(it != scene.rend())
	{
		// find the correcsponding node in the selection if there is one
		MNodeIter selIt = sceneSelection.find((*it));

		if(selIt != sceneSelection.end())
		{
			if(it == scene.rbegin())
				break;

			itLast = it;
			it--;
			scene.move_backward(itLast);
			ret = true;
		}
		else
		{
			it--;
		}
	}

	if(ret)
	{
		SetChanged();
		return true;
	}

	return false;
}

int PQMap::SelBringToFront()
{
	int stepsForward = 0;
	while(SelBringForward())
	{
		stepsForward++;
	}

	if(stepsForward > 0)
		SetChanged();

	return stepsForward;
}

bool PQMap::SelSendBack()
{
	bool ret = false;

	MNodeIter itLast;
	MNodeIter it = scene.begin(); //rbegin = front
	while(it != scene.end())
	{
		// find the correcsponding node in the selection if there is one
		MNodeIter selIt = sceneSelection.find((*it));

		if(selIt != sceneSelection.end())
		{
			if(it == scene.begin())
				break;

			itLast = it;
			it++;
			scene.move_forward(itLast);
			ret = true;
		}
		else
		{
			it++;
		}
	}

	if(ret)
	{
		SetChanged();
		return true;
	}

	return false;
}

int PQMap::SelSendToBack()
{
	int stepsBackward = 0;
	while(SelSendBack())
	{
		stepsBackward++;
	}

	if(stepsBackward > 0)
		SetChanged();

	return stepsBackward;
}

bool PQMap::AddResource(const PQResourcePtr &res)
{
	if(!res)
		return false;

	if(resources.size() >= MAX_RESOURCES)
	{
		MessageBox(0, "Can't add any more resources. Please delete some before trying to add more.", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// add resource to list
	resources.push_back(res);

	// signal editor to require save
	SetChanged();

	// successfully added resource
	return true;
}

bool PQMap::RemoveResource(const PQResourcePtr &res)
{
	if(!res)
		return false;

	// find the resource and delete it
	for(Uint16 i = 0; i < resources.size(); i++)
	{
		if(resources[i] == res) // found the resource**
		{
			bool deleteFromScene = false;

			// delete any nodes in the scene using this resource
			MNodeIter sIt = scene.begin();
			while(sIt != scene.end())
			{
				if((*sIt)->res == res)
				{
					if(deleteFromScene == false) // check if the user has already chosen to delete from scene
					{
						if(IDCANCEL == MessageBox(0, "This resource is currently being used in the map.\nAll instances of this resource in the scene will be deleted...",
													 "Deleting Resource", MB_OKCANCEL | MB_ICONQUESTION))
							return false;

						deleteFromScene = true;
					}

					// if scene node is in selection, remove it
					MNodeIter selIt = sceneSelection.begin();
					while(selIt != sceneSelection.end())
					{
						if((*selIt) == (*sIt))
						{
							sceneSelection.erase(selIt);
							break;
						}
						else
						{
							selIt++;
						}
					}

					// remove from scene
					sIt = scene.erase(sIt);
				}
				else
				{
					sIt++;
				}
			}

			// finally remove the item from the resources
			resources.erase(resources.begin() + i);

			SetChanged();

			return true;
		}
	}

	return false;
}

bool PQMap::RemoveUnusedResources()
{
	bool deletedResources = false;

	for(auto it = resources.begin();
		it != resources.end(); )
	{
		bool inScene = false;

		for(auto& node : scene)
		{
			if(node->res.get() == it->get())
			{
				inScene = true;
				break;
			}
		}

		if(!inScene)
		{
			it = resources.erase(it);
			deletedResources = true;
			SetChanged();
		}
		else
		{
			++it;
		}
	}

	return deletedResources;
}

int PQMap::NumResources()
{
	return resources.size();
}

int PQMap::SaveMap(Filename fname)
{
	int ret = 0;
	
	// exit if there is no name to save to
	if(fname == "")
	{
		if(filename != "")
			fname = filename;
		else
			return -1;
	}

	// exit if filename is too long
	if(fname.size() > MAX_PATH_LENGTH - 1)
		return -1;

	//read all data from map
	ofstream fout;
	
	try
	{
		// FAIL IF THE FILE COULD NOT BE OPENED
		fout.open(fname.c_str(), ios_base::out | ios_base::binary);
		
		if(!fout.is_open() || !fout.good())
			throw "The map file could not be created.";
		
// MAP HEADER
	// MAP FILE VERSION
		fout.write((const char*)&PQ_MAP_FILE_MAX_VERSION, sizeof(Uint8));
	// TIME LIMIT
		fout.write((const char*)&timeLimit, sizeof(unsigned int));
	// NUMBER OF DELIVERIES
		fout.write((const char*)&numDeliveries, sizeof(int));
	// MIN X
		fout.write((const char*)&vRuler[0].x, sizeof(float));
	// MAX X
		fout.write((const char*)&vRuler[1].x, sizeof(float));
	// MIN Y
		fout.write((const char*)&hRuler[0].y, sizeof(float));
	// MAX Y
		fout.write((const char*)&hRuler[1].y, sizeof(float));

	// NUMBER OF RESOURCES
		int nResources = (int)resources.size();
		fout.write((const char*)&nResources, sizeof(int));

	// NUMBER OF NODES
		int nNodes = (int)scene.size();
		fout.write((const char*)&nNodes, sizeof(int));

// RESOURCES
		for(int i = 0; i < nResources; i++)
		{
	// RESOURCE TYPE
			Uint8 resType = resources[i]->type;
			fout.write((const char*)&resType, sizeof(Uint8));

		// make filename relative
			char sourceBuff[260];

			Filename fn = resources[i]->source_file;
			fn.MakeRelativeTo(fname);

			memset(sourceBuff, 0, sizeof(sourceBuff));
			memcpy(sourceBuff, fn.c_str(), fn.size() + 1);
	// RESOURCE SOURCE FILE
			fout.write((const char*)sourceBuff, sizeof(sourceBuff));
	// RESOURCE NAME
			fout.write((const char*)resources[i]->resource_name, sizeof(resources[i]->resource_name));
	// RESOURCE CLASS ID
			fout.write((const char*)resources[i]->class_id, sizeof(resources[i]->class_id));

			switch(resources[i]->type)
			{
				case N_RES_IMAGE:
				{
					PQResImagePtr rImg = dynamic_pointer_cast<PQResImage>(resources[i]);
		// RESOURCE NUMBER OF ROWS
					fout.write((const char*)&rImg->nRows, sizeof(Uint16));
		// RESOURCE NUMBER OF COLUMNS
					fout.write((const char*)&rImg->nCols, sizeof(Uint16));
		
		// RESOURCE NUMBER OF COLLISION SHAPES
					int nShapes = (int)rImg->collision_shapes.size();
					fout.write((const char*)&nShapes, sizeof(int));

		// RESOURCE COLLISION SHAPES
					for(int s = 0; s < nShapes; s++)
					{
			// SHAPE TYPE
						Uint8 shapeType = rImg->collision_shapes[s]->type;
						fout.write((const char*)&shapeType, sizeof(Uint8));

						switch(shapeType)
						{
							case SHAPE_CIRCLE:
							{
								PQB2CirclePtr circle = dynamic_pointer_cast<PQB2Circle>(rImg->collision_shapes[s]);

					// CIRCLE CENTER
								fout.write((const char*)&circle->center, sizeof(Vec2<float>));
					// CIRCLE RADIUS
								fout.write((const char*)&circle->radius, sizeof(float));

								break;
							}
							case SHAPE_POLYGON:
							{
								PQB2PolygonPtr polygon = dynamic_pointer_cast<PQB2Polygon>(rImg->collision_shapes[s]);

					// POLYGON NUMBER OF VERTICES
								fout.write((const char*)&polygon->nVerts, sizeof(int));
					// POLYGON VERTICES
								fout.write((const char*)polygon->vertices, polygon->nVerts * sizeof(Vec2<float>));

								break;
							}
							case SHAPE_BOX:
							{
								PQB2BoxPtr box = dynamic_pointer_cast<PQB2Box>(rImg->collision_shapes[s]);

					// BOX CENTER
								fout.write((const char*)&box->center, sizeof(Vec2<float>));
					// BOX HALF-WIDTH
								fout.write((const char*)&box->halfWidth, sizeof(float));
					// BOX HALF-HEIGHT
								fout.write((const char*)&box->halfHeight, sizeof(float));
					// BOX ANGLE
								fout.write((const char*)&box->angle, sizeof(float));

								break;
							}
							case SHAPE_EDGE:
							{
								PQB2EdgePtr edge = dynamic_pointer_cast<PQB2Edge>(rImg->collision_shapes[s]);

					// EDGE VERTEX 1
								fout.write((const char*)&edge->p1, sizeof(Vec2<float>));
					// EDGE VERTEX 2
								fout.write((const char*)&edge->p2, sizeof(Vec2<float>));

								break;
							}
						}
					}

					break;
				}
				case N_RES_SOUND:
				{
					PQResSoundPtr rSnd = dynamic_pointer_cast<PQResSound>(resources[i]);
					// no properties yet

					break;
				}
			}
		}

// NODES
		MNodeIter itScene = scene.begin();
		while(itScene != scene.end())
		{
			PQMapNodePtr node = (*itScene);

		// retrieve resource index
			for(Uint8 i = 0; i < resources.size(); i++)
			{
				if(node->res == resources[i])
				{
					node->resIndex = i;
					break;
				}
			}

			assert(node->resIndex != 255);

	// NODE TYPE
			fout.write((const char*)&node->type, sizeof(Uint8));
	// NODE RESOURCE INDEX
			fout.write((const char*)&node->resIndex, sizeof(Uint8));
	// NODE POSITION
			fout.write((const char*)&node->position, sizeof(Vec2<float>));

			switch(node->type)
			{
				case N_MAP_IMAGE:
				{
					PQMapImagePtr mImg = dynamic_pointer_cast<PQMapImage>(node);
		// NODE ROW
					fout.write((const char*)&mImg->row, sizeof(Uint8));
		// NODE COLUMN
					fout.write((const char*)&mImg->col, sizeof(Uint8));
		// NODE ANGLE
					fout.write((const char*)&mImg->angle, sizeof(float));
		// NODE SCALE
					fout.write((const char*)&mImg->scale, sizeof(float));
		// NODE VALUE1
					fout.write((const char*)&mImg->value1, sizeof(Int16));
		// NODE VALUE2
					fout.write((const char*)&mImg->value2, sizeof(Int16));
					break;
				}
				case N_MAP_SOUND:
				{
					PQMapSoundPtr mSnd = dynamic_pointer_cast<PQMapSound>(node);
		// NODE VOLUME
					fout.write((const char*)&mSnd->volume, sizeof(float));
		// NODE LOOPING
					fout.write((const char*)&mSnd->loop, sizeof(bool));
		// NODE DELAY LOOP DELAY
					fout.write((const char*)&mSnd->delay, sizeof(int));
		// NODE TRIGGER RADIUS
					fout.write((const char*)&mSnd->triggerRadius, sizeof(int));
					break;
				}
			}

			itScene++;
		}

// NUMBER OF PEDESTRIAN GRAPH NODES
		int nPedGraphNodes = (int)pedGraph.size();
		fout.write((const char*)&nPedGraphNodes, sizeof(int));

// PEDESTRIAN GRAPH NODES
		for(size_t i = 0; i < pedGraph.size(); i++)
		{
	// PEDESTRIAN GRAPH NODE POSITION
			fout.write((const char*)&pedGraph[i]->position, sizeof(Vec2<float>));
			
	// PEDESTRIAN GRAPH NODE IS-DESTINATION FLAG
			fout.write((const char*)&pedGraph[i]->isDestination, sizeof(bool));

			// temp index for making adjacency list
			pedGraph[i]->tmpIndex = i; // don't write
		}

// PEDESTRIAN GRAPH ADJACENCY LISTS
		for(size_t i = 0; i < pedGraph.size(); i++)
		{
	// NUMBER OF NEIGHBOUR NODES
			int nNeighbours = pedGraph[i]->neighbours.size();
			fout.write((const char*)&nNeighbours, sizeof(int));

		// NEIGHBOUR INDICES
			for(size_t j = 0; j < pedGraph[i]->neighbours.size(); j++)
			{
			// INDEX OF THIS NEIGHBOUR
				fout.write((const char*)&pedGraph[i]->neighbours[j]->tmpIndex, sizeof(int));
			}
		}

// NUMBER OF VEHICLE GRAPH NODES
		int nVehGraphNodes = (int)vehGraph.size();
		fout.write((const char*)&nVehGraphNodes, sizeof(int));

// VEHICLE GRAPH NODES
		for(size_t i = 0; i < vehGraph.size(); i++)
		{
	// VEHICLE GRAPH NODE POSITION
			fout.write((const char*)&vehGraph[i]->position, sizeof(Vec2<float>));
			
	// VEHICLE GRAPH NODE IS-DESTINATION FLAG
			fout.write((const char*)&vehGraph[i]->isDestination, sizeof(bool));

			// temp index for making adjacency list
			vehGraph[i]->tmpIndex = i; // don't write
		}

// VEHICLE GRAPH ADJACENCY LISTS
		for(size_t i = 0; i < vehGraph.size(); i++)
		{
	// NUMBER OF NEIGHBOUR NODES
			int nNeighbours = vehGraph[i]->neighbours.size();
			fout.write((const char*)&nNeighbours, sizeof(int));

		// NEIGHBOUR INDICES
			for(size_t j = 0; j < vehGraph[i]->neighbours.size(); j++)
			{
			// INDEX OF THIS NEIGHBOUR
				fout.write((const char*)&vehGraph[i]->neighbours[j]->tmpIndex, sizeof(int));
			}
		}

		if(fout.fail())
			throw "Could not write to the map file: scene";

		fout.close();

		hasChanged = false;
		filename = fname;
	}

	catch(const char* err)
	{
		MessageBox(0, err, "Error", MB_OK | MB_ICONERROR);
		fout.close();
		ret = -1;
	}

	return ret;
}

int PQMap::LoadMap(Filename fname)
{
	// RESET EDITOR BEFORE LOADING MAP
	Initialize();

	int ret = 0;

	ifstream fin;

	try
	{
		// FAIL IF THE FILE COULD NOT BE OPENED
		fin.open(fname, ios_base::in | ios_base::binary);
		if(!fin.is_open() || !fin.good())
			throw "The map file could not be opened.";
		
// MAP HEADER
	// VERSION
		Uint8 mapVers;
		fin.read((char*)&mapVers, sizeof(Uint8));

		if(fin.fail())
			throw "Could not read from the map file";
		else if(mapVers < PQ_MAP_FILE_MIN_VERSION)
			throw "The specified map file is too old, and cannot be opened.";
		else if(mapVers > PQ_MAP_FILE_MAX_VERSION)
			throw "Please update Pizza Quest Map Editor before opening this map file.";
		 
		if(mapVers >= 140)
		{
	// TIME LIMIT
			fin.read((char*)&timeLimit, sizeof(unsigned int));

	// NUMBER OF DELIVERIES
			fin.read((char*)&numDeliveries, sizeof(int));
		}

		if(mapVers >= 141)
		{
			// MIN X
				fin.read((char*)&vRuler[0].x, sizeof(float));
			// MAX X
				fin.read((char*)&vRuler[1].x, sizeof(float));
			// MIN Y
				fin.read((char*)&hRuler[0].y, sizeof(float));
			// MAX Y
				fin.read((char*)&hRuler[1].y, sizeof(float));
		}

		// NUMBER OF RESOURCES
		int nResources;
		fin.read((char*)&nResources, sizeof(int));

		int nObjects;

		if(mapVers >= 142)
		{
			fin.read((char*)&nObjects, sizeof(int));
		}

		// temporaries
		Uint16 nRows;
		Uint16 nCols;
		char source_file[260];
		char resource_name[32];
		char class_id[32];

// RESOURCES
		for(int i = 0; i < nResources; i++)
		{
	// RESOURCE TYPE
			Uint8 type;
			fin.read((char*)&type, sizeof(Uint8));

			// convert types for old maps
			if(mapVers < 120)
			{
				switch(type)
				{
				case RES_IMAGE:
					type = N_RES_IMAGE;
					break;
				case RES_SOUND:
					type = N_RES_SOUND;
					break;
				}
			}

			switch(type)
			{
				case N_RES_IMAGE:
				{
			// RESOURCE SOURCE FILE
					fin.read((char*)&source_file, sizeof(source_file));

					Filename fn = source_file;
					fn.EraseLeadingSlash();
					fn.MakeAbsoluteFrom(fname);

					if(fn.Exists() == false)
					{
						Filename newFile = FindFile(fn.GetName(), fname.GetPath());

						if(newFile != "")
						{

							string message = "File not found:\n";
							message += fn;
							message += "\n\n";
							message += "Would you like to use this file instead?\n";
							message += newFile;

							int mbRet = MessageBox(GetActiveWindow(), message.c_str(), "File not found not", MB_YESNOCANCEL | MB_ICONQUESTION);

							switch(mbRet)
							{
							case IDYES:
								fn = newFile;
								newFile.CopyTo(source_file, 260);
								break;

							case IDNO:
								fn = "";
								break;

							case IDCANCEL:
								fin.close();
								Initialize();
								return 0;
							}
						}
						else
						{
							fn.insert(0, "File not found: ");
							MessageBox(GetActiveWindow(), fn.c_str(), "File not found!", MB_OK | MB_ICONERROR);
							fn = "";
						}
					}
					
			// RESOURCE NAME
					fin.read((char*)&resource_name, sizeof(resource_name));
			// RESOURCE CLASS ID
					fin.read((char*)&class_id, sizeof(class_id));
			// RESOURCE NUMBER OF ROWS
					fin.read((char*)&nRows, sizeof(Uint16));
			// RESOURCE NUMBER OF COLUMNS
					fin.read((char*)&nCols, sizeof(Uint16));

			// CREATE THE RESOURCE
					PQResImagePtr rImg( new PQResImage(fn, resource_name, class_id, nRows, nCols) );

			// COLLISION SHAPES
					if(mapVers >= 110) // collision shapes not included before this version
					{
				// NUMBER OF COLLISION SHAPES
						int nShapes;
						fin.read((char*)&nShapes, sizeof(int));
				// COLLISION SHAPES
						for(int s = 0; s < nShapes; s++)
						{
					// SHAPE TYPE
							Uint8 shapeType;
							fin.read((char*)&shapeType, sizeof(Uint8));

							switch(shapeType)
							{
								case SHAPE_CIRCLE:
								{
									PQB2Circle *circle = new PQB2Circle;

							// CIRCLE CENTER
									fin.read((char*)&circle->center, sizeof(Vec2<float>));
							// CIRCLE RADIUS
									fin.read((char*)&circle->radius, sizeof(float));

									rImg->collision_shapes.push_back( PQB2CirclePtr(circle) );

									break;
								}
								case SHAPE_POLYGON:
								{
									PQB2Polygon *polygon = new PQB2Polygon;

							// POLYGON NUMBER OF VERTICES
									fin.read((char*)&polygon->nVerts, sizeof(int));
							// POLYGON VERTICES
									fin.read((char*)polygon->vertices, polygon->nVerts * sizeof(Vec2<float>));

									rImg->collision_shapes.push_back( PQB2PolygonPtr(polygon) );

									break;
								}
								case SHAPE_BOX:
								{
									PQB2Box *box = new PQB2Box;

							// BOX CENTER
									fin.read((char*)&box->center, sizeof(Vec2<float>));
							// BOX HALF-WIDTH
									fin.read((char*)&box->halfWidth, sizeof(float));
							// BOX HALF-HEIGHT
									fin.read((char*)&box->halfHeight, sizeof(float));
							// BOX ANGLE
									fin.read((char*)&box->angle, sizeof(float));

									rImg->collision_shapes.push_back( PQB2BoxPtr(box) );

									break;
								}
								case SHAPE_EDGE:
								{
									PQB2Edge *edge = new PQB2Edge;

							// EDGE VERTEX 1
									fin.read((char*)&edge->p1, sizeof(Vec2<float>));
							// EDGE VERTEX 2
									fin.read((char*)&edge->p2, sizeof(Vec2<float>));

									rImg->collision_shapes.push_back( PQB2EdgePtr(edge) );

									break;
								}
							}
						}
					}

					resources.push_back(rImg);
					break;
				}
				case N_RES_SOUND:
				{
			// RESOURCE SOURCE FILE
					fin.read((char*)&source_file, sizeof(source_file));

					// make the file name absolute and set it back in
					Filename fn = source_file;
					fn.EraseLeadingSlash();
					fn.MakeAbsoluteFrom(fname);

					if(fn.Exists() == false)
					{
						Filename newFile = FindFile(fn.GetName(), fname.GetPath());

						if(newFile != "")
						{
							string message = "File not found:\n";
							message += fn;
							message += "\n";
							message += "Would you like to use this file instead?\n";
							message += newFile;

							int mbRet = MessageBox(GetActiveWindow(), message.c_str(), "File not found not", MB_YESNOCANCEL | MB_ICONQUESTION);

							switch(mbRet)
							{
							case IDYES:
								fn = newFile;
								break;

							case IDNO:
								fn = "";
								break;

							case IDCANCEL:
								fin.close();
								Initialize();
								return 0;
							}
						}
						else
						{
							fn.insert(0, "File not found: ");
							MessageBox(GetActiveWindow(), fn.c_str(), "File not found!", MB_OK | MB_ICONERROR);
							fn = "";
						}
					}

			// RESOURCE NAME
					fin.read((char*)&resource_name, sizeof(resource_name));
			// RESOURCE CLASS ID
					fin.read((char*)&class_id, sizeof(class_id));

			// NO SPECIFIC PROPERTIES YET
					//...

			// CREATE THE RESOURCE
					PQResSoundPtr rSnd( new PQResSound(fn, resource_name, class_id) );

					resources.push_back(rSnd);
					break;
				}
			}
		}

// NUMBER OF NODES
		
		if(mapVers <= 141)
		{
			fin.read((char*)&nObjects, sizeof(int));
		}

// NODES
		Vec2<float> position;
		Vec2<int> position_i;
		Uint8 resIndex;

		for(int i = 0; i < nObjects; i++)
		{
		// OBJECT TYPE
			Uint8 type;
			fin.read((char*)&type, sizeof(Uint8));
		// OBJECT RESOURCE INDEX
			fin.read((char*)&resIndex, sizeof(Uint8));

			assert(resIndex >= 0 && resIndex < nObjects);

			// convert types for old maps
			if(mapVers < 120)
			{
				switch(type)
				{
				case MAP_IMAGE:
					type = N_MAP_IMAGE;
					break;
				case MAP_SOUND:
					type = N_MAP_SOUND;
					break;
				}

			// OBJECT POSITION (position was <int> before version 1.2)
				fin.read((char*)&position_i, sizeof(Vec2<int>));
				position.set((float)position_i.x, (float)position_i.y);
			}
			else
			{
			// OBJECT POSITION
				fin.read((char*)&position, sizeof(Vec2<float>));
			}

			//OutputDebugString(PQString(position.x) + " " + PQString(position.y) + "\n");

			switch(type)
			{
				case N_MAP_IMAGE:
				{
			// GET RESOURCE POINTER
					PQResImagePtr res = dynamic_pointer_cast<PQResImage>( resources[resIndex] );

			// OBJECT ROW
					fin.read((char*)&res->newRow, sizeof(Uint8));
			// OBJECT COLUMN
					fin.read((char*)&res->newCol, sizeof(Uint8));
			// OBJECT ANGLE
					fin.read((char*)&res->newAngle, sizeof(float));
			// OBJECT SCALE
					fin.read((char*)&res->newScale, sizeof(float));
			// OBJECT VALUE1
					fin.read((char*)&res->newValue1, sizeof(Int16));
			// OBJECT VALUE2
					fin.read((char*)&res->newValue2, sizeof(Int16));

					PQMapImagePtr mImg( new PQMapImage(dynamic_pointer_cast<PQResource>(res)) );
					mImg->SetPosition(position);

					scene.push_back(mImg);
				
					break;
				}
				case N_MAP_SOUND:
				{
			// GET RESOURCE POINTER
					PQResSoundPtr res = dynamic_pointer_cast<PQResSound>( resources[resIndex] );
					if(mapVers >= 141) // DATA TYPES CHANGED AFTER 141
					{
				// OBJECT VOLUME
						fin.read((char*)&res->newVolume, sizeof(float));
				// OBJECT LOOPING
						fin.read((char*)&res->newLoop, sizeof(bool));
				// OBJECT LOOP DELAY
						fin.read((char*)&res->newDelay, sizeof(int));
				// OBJECT TRIGGER RADIUS
						fin.read((char*)&res->newTriggerRadius, sizeof(int));
					}
					else
					{
				// OBJECT VOLUME
						Uint8 vol;
						fin.read((char*)&vol, sizeof(Uint8));
						res->newVolume = (float)vol;
				// OBJECT LOOPING
						fin.read((char*)&res->newLoop, sizeof(bool));
				// OBJECT LOOP DELAY
						Uint16 delay;
						fin.read((char*)&delay, sizeof(Uint16));
						res->newDelay = (int)delay;
				// OBJECT TRIGGER RADIUS
						Uint16 trigRad;
						fin.read((char*)&trigRad, sizeof(Uint16));
						res->newTriggerRadius = (int)trigRad;
					}

					PQMapSoundPtr mSnd( new PQMapSound(dynamic_pointer_cast<PQResource>(res)) );
					mSnd->SetPosition(position);

					scene.push_back(mSnd);

					break;
				}
				default:
					assert(0);
					break;
			}
		}

		int ss = scene.size();

// GRAPH NODES ONLY SUPPORTED AFTER VERSION 140
		if(mapVers >= 140)
		{
	// NUMBER OF PEDESTRIAN GRAPH NODES
			int nPedGraphNodes;
			fin.read((char*)&nPedGraphNodes, sizeof(int));
			
			if(nPedGraphNodes > 0)
				pedGraph.reserve(nPedGraphNodes);

	// PEDESTRIAN GRAPH NODES
			for(int i = 0; i < nPedGraphNodes; i++)
			{	
		// PEDESTRIAN GRAPH NODE POSITION
				fin.read((char*)&position, sizeof(Vec2<float>));
			
				PQPedGraphNodePtr pGrph( new PQPedGraphNode(position.x, position.y) );

		// PEDESTRIAN GRAPH NODE IS-DESTINATION FLAG
				fin.read((char*)&pGrph->isDestination, sizeof(bool));

				pedGraph.push_back( pGrph );
			}

	// PEDESTRIAN GRAPH ADJACENCY LISTS
			int nNeighbours;
			int neighbourIndex;
			for(int i = 0; i < nPedGraphNodes; i++)
			{
		// NUMBER OF NEIGHBOUR NODES
				fin.read((char*)&nNeighbours, sizeof(int));
				pedGraph[i]->neighbours.reserve(nNeighbours);

				// NEIGHBOUR INDICES
				for(int j = 0; j < nNeighbours; j++)
				{
				// INDEX OF THIS NEIGHBOUR
					fin.read((char*)&neighbourIndex, sizeof(int));

					// add neighbour to list
					pedGraph[i]->neighbours.push_back( pedGraph[neighbourIndex] );
				}
			}

	// NUMBER OF VEHICLE GRAPH NODES
			int nVehGraphNodes;
			fin.read((char*)&nVehGraphNodes, sizeof(int));
			
			if(nVehGraphNodes > 0)
				vehGraph.reserve(nVehGraphNodes);

	// VEHICLE GRAPH NODES
			for(int i = 0; i < nVehGraphNodes; i++)
			{	
		// VEHICLE GRAPH NODE POSITION
				fin.read((char*)&position, sizeof(Vec2<float>));
			
				PQVehGraphNodePtr vGrph( new PQVehGraphNode(position.x, position.y) );

		// VEHICLE GRAPH NODE IS-DESTINATION FLAG
				fin.read((char*)&vGrph->isDestination, sizeof(bool));

				vehGraph.push_back( vGrph );
			}

	// VEHICLE GRAPH ADJACENCY LISTS
			for(int i = 0; i < nVehGraphNodes; i++)
			{
		// NUMBER OF NEIGHBOUR NODES
				int nNeighbours;
				fin.read((char*)&nNeighbours, sizeof(int));
				vehGraph[i]->neighbours.reserve(nNeighbours);

				// NEIGHBOUR INDICES
				int neighbourIndex;
				for(int j = 0; j < nNeighbours; j++)
				{
				// INDEX OF THIS NEIGHBOUR
					fin.read((char*)&neighbourIndex, sizeof(int));

					// add neighbour to list
					vehGraph[i]->neighbours.push_back( vehGraph[neighbourIndex] );
				}
			}
		}

		if(fin.fail())
			throw "Could not load the map file";

		fin.close();
		filename = fname;
	}
	catch(const char* err)
	{
		MessageBox(0, err, "Error", MB_OK | MB_ICONERROR);
		fin.close();
		fin.clear();
		ret = -1;
	}

	return ret;
}

Filename PQMap::FindFile(const Filename &filename, const Filename &path)
{
	list<string> dirs;
	
	dirs.push_back(path.GetPath() + "\\*.*");

	while( !dirs.empty() )
	{
		Filename dir = dirs.front();
		dirs.pop_front();

		CFileFind ff;
		BOOL good = ff.FindFile( dir.c_str() );
		
		// go through all the files in this directory,
		// adding new directories as they are found
		while(good)
		{
			// get next subitem
			good = ff.FindNextFile();

			if( ff.IsDots() )
				continue;

			if( ff.IsDirectory() )
			{
				// add new directory to the list
				Filename foundDir = Filename(ff.GetFilePath() + "\\*.*");
				dirs.push_back(foundDir);
			}
			else
			{
				Filename foundFile = (const char*)ff.GetFileName();
				if(foundFile == filename)
				{
					Filename retFile = (const char*)ff.GetFilePath();
					ff.Close();
					return retFile;
				}
			}
		}

		ff.Close();
	}

	return "";
}
