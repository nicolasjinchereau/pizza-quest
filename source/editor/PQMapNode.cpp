/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "pqmapnode.h"

GL_SpritePtr PQResSound::img;

/*****************************
PQ NODE
*****************************/

PQNode::PQNode(Uint8 type) : type(type)
{

}

PQNode::PQNode(const PQNode &that)
{
	this->type = that.type;
}

PQNode::~PQNode()
{

}

void PQNode::Swap(Undoable *object)
{
	PQNode &obj = *(PQNode*)object;
	SWAP<Uint8>(this->type, obj.type);
}

/*****************************
PQ RESOURCE
*****************************/

PQResource::PQResource(Uint8 type) : PQNode(type)
{
	memset(source_file, 0, sizeof(source_file));
	memset(resource_name, 0, sizeof(resource_name));
	memset(class_id, 0, sizeof(class_id));

	tmpIndex = -1;
}

PQResource::PQResource(const PQResource &that) : PQNode(that)
{
	memcpy(this->source_file, that.source_file, sizeof(source_file));
	memcpy(this->resource_name, that.resource_name, sizeof(resource_name));
	memcpy(this->class_id, that.class_id, sizeof(class_id));

	this->tmpIndex = that.tmpIndex;
}

PQResource::~PQResource()
{

}

void PQResource::Swap(Undoable *object)
{
	PQNode::Swap(object);

	PQResImage &obj = *(PQResImage*)object;

	char tmp[260];

	memcpy(tmp, obj.source_file, sizeof(source_file));
	memcpy(obj.source_file, this->source_file, sizeof(source_file));
	memcpy(this->source_file, tmp, sizeof(source_file));

	memcpy(tmp, obj.resource_name, sizeof(resource_name));
	memcpy(obj.resource_name, this->resource_name, sizeof(resource_name));
	memcpy(this->resource_name, tmp, sizeof(resource_name));

	memcpy(tmp, obj.class_id, sizeof(class_id));
	memcpy(obj.class_id, this->class_id, sizeof(class_id));
	memcpy(this->class_id, tmp, sizeof(class_id));
}

void PQResource::SetResourceName(const char *resource_name)
{
	strcpy_s(this->resource_name, resource_name);
}

void PQResource::SetClassID(const char *class_id)
{
	strcpy_s(this->class_id, class_id);
}

/*****************************
PQ RES IMAGE
*****************************/

PQResImage::PQResImage(string sourceFile, string resourceName, string classID, Uint16 nRows, Uint16 nCols)
	 : PQResource(N_RES_IMAGE)
{
	// copy all the initial resource data
	strcpy_s(source_file, sourceFile.c_str());
	strcpy_s(resource_name, resourceName.c_str());
	strcpy_s(class_id, classID.c_str());

	// image resource properties
	this->nRows = nRows;
	this->nCols = nCols;

	// open the resource image
	if(string(source_file) != "")
	{
		img.reset( new GL_Sprite(source_file) );
		img->SetNumRows(nRows);
		img->SetNumCols(nCols);	
	}
	else
	{
		img.reset( new GL_Sprite(IDB_MISSING) );
	}

	// initialize all the properties given out to new nodes
	newRow = 0;
	newCol = 0;
	newAngle = 0;
	newScale = 1.0f;
	newValue1 = 0;
	newValue2 = 0;
}

PQResImage::PQResImage(const PQResImage &that) : PQResource(that)
{
	this->nRows = that.nRows;
	this->nCols = that.nCols;

	this->collision_shapes = that.collision_shapes;
	this->img = that.img;

	this->newRow = that.newRow;
	this->newCol = that.newCol;
	this->newAngle = that.newAngle;
	this->newScale = that.newScale;
	this->newValue1 = that.newValue1;
	this->newValue2 = that.newValue2;
}

PQResImage::~PQResImage()
{

}

Undoable *PQResImage::GetCopy()
{
	return new PQResImage(*this);
}

void PQResImage::Swap(Undoable *object)
{
	PQResource::Swap(object);

	PQResImage &obj = *(PQResImage*)object;

	SWAP<Uint16>(this->nRows, obj.nRows);
	SWAP<Uint16>(this->nCols, obj.nCols);

	SWAP< vector<PQB2ShapePtr> >(this->collision_shapes, obj.collision_shapes);
	SWAP<GL_SpritePtr>(this->img, obj.img);

	SWAP<Uint8>(this->newRow, obj.newRow);
	SWAP<Uint8>(this->newCol, obj.newCol);
	SWAP<float>(this->newAngle, obj.newAngle);
	SWAP<float>(this->newScale, obj.newScale);
	SWAP<Int16>(this->newValue1, obj.newValue1);
	SWAP<Int16>(this->newValue2, obj.newValue2);
}

void PQResImage::SetSourceFile(const char *sourceFile)
{
	// copy the resource filename
	strcpy_s(source_file, sizeof(source_file), sourceFile);

	// load image
	img.reset( new GL_Sprite(source_file) );

	// set current image properties
	img->SetNumRows(nRows);
	img->SetNumCols(nCols);
}

long PQResImage::GetDataSize()
{
	return sizeof(Uint8) +		// type
		   sizeof(source_file) +
		   sizeof(resource_name) +
		   sizeof(class_id) +
		   sizeof(Uint16) +		// rows
		   sizeof(Uint16);		// columns
}

void PQResImage::Draw(GL_Screen *dest, float x, float y, Uint8 Row, Uint8 Col, float Scale, float Angle, float shade, float alpha)
{
	//if(img)
	//{
	img->SetPos(x, y);
	img->SetScale(Scale);
	img->SetAngle(Angle);
	img->SetRow(Row);
	img->SetColumn(Col);
	img->Draw(dest, NULL, shade, alpha);
	//}
	//else
	//{
	//	const float w = 100 * Scale;
	//	const float h = 100 * Scale;
	//	Rect<float> rcMissing(x, y, 100 * Scale, 100 * Scale);
	//	GL_FillRect(dest, &rcMissing, Color(255, 0, 0));
	//}
}

void PQResImage::DrawThumb(GL_Screen *dest)
{
	float sx;
	float sy;
	int border = 5;
	//if(img)
	//{	
		sx = (float)(dest->GetWidth() - (2 * border)) / (float)img->GetWidth();
		sy = (float)(dest->GetHeight() - (2 * border)) / (float)img->GetHeight();
	//}
	//else
	//{
	//	sx = (float)(dest->GetWidth() - (2 * border)) / 100.0f;
	//	sy = (float)(dest->GetHeight() - (2 * border)) / 100.0f;
	//}

	float scale = MIN(sx, sy);

	float hx = (float)dest->GetWidth() / 2.0f;
	float hy = (float)dest->GetHeight() / 2.0f;

	Draw(dest, hx, hy, newRow, newCol, scale, newAngle);

	for(int i = 0; i < (int)collision_shapes.size(); i++)
		collision_shapes[i]->Draw(dest, hx, hy, scale);
}

/*****************************
PQ RES SOUND
*****************************/

PQResSound::PQResSound(string sourceFile, string resourceName, string classID)
	 : PQResource(N_RES_SOUND)
{
	strcpy_s(source_file, sourceFile.c_str());
	strcpy_s(resource_name, resourceName.c_str());
	strcpy_s(class_id, classID.c_str());

	// load the speaker for the thumbnail
	if( !img )
		img.reset( new GL_Sprite(IDB_SOUND) );

	// initialize all the properties given out to new nodes
	newVolume = 1.0f;
	newLoop = false;
	newDelay = 0;
	newTriggerRadius = 75;
}

PQResSound::PQResSound(const PQResSound &that) : PQResource(that)
{
	// img must already be loaded if copy ctor is being called
	this->newVolume = that.newVolume;
	this->newLoop = that.newLoop;
	this->newDelay = that.newDelay;
	this->newTriggerRadius = that.newTriggerRadius;
}

PQResSound::~PQResSound()
{

}

Undoable *PQResSound::GetCopy()
{
	return new PQResSound(*this);
}

void PQResSound::Swap(Undoable *object)
{
	PQResource::Swap(object);

	PQResSound &obj = *(PQResSound*)object;

	SWAP<float>( this->newVolume, obj.newVolume );
	SWAP<bool>( this->newLoop, obj.newLoop );
	SWAP<int>( this->newDelay, obj.newDelay );
	SWAP<int>( this->newTriggerRadius, obj.newTriggerRadius );
}

void PQResSound::SetSourceFile(const char *sourceFile)
{
	// delete currently loaded sound
	//...

	// copy the resource filename
	strcpy_s(this->source_file, sourceFile);

	// load sound
	//...

	// set current sound properties
	//...
}

long PQResSound::GetDataSize()
{
	return sizeof(Uint8); // type
}

void PQResSound::Draw(GL_Screen *dest, float x, float y, Uint8 Row, Uint8 Col, float Scale, float Angle, float shade, float alpha)
{
	img->SetPos(x, y);
	img->SetScale(Scale);
	img->SetAngle(Angle);
	img->SetRow(Row);
	img->SetColumn(Col);
	img->Draw(dest, NULL, shade, alpha);
}

void PQResSound::DrawThumb(GL_Screen *dest)
{
	float hx = (float)dest->GetWidth() / 2.0f;
	float hy = (float)dest->GetHeight() / 2.0f;
	Draw(dest, hx, hy, 0, 0, 1.0f, 0);
}

/*****************************
PQ MAP NODE
*****************************/

PQMapNode::PQMapNode(Uint8 type) : PQNode(type)
{
	res.reset();
	selected = false;
	resIndex = 255;
}

PQMapNode::PQMapNode(const PQMapNode &that) : PQNode(that)
{
	this->position = that.position;
	this->res = that.res;
	this->rect = that.rect;
	this->selected = that.selected;
}

void PQMapNode::Swap(Undoable *object)
{
	PQNode::Swap(object);

	PQMapNode &obj = *(PQMapNode*)object;
	
	SWAP< Vec2<float> >( this->position, obj.position );
	SWAP<PQResourcePtr>( this->res, obj.res );
	SWAP< Rect<float> >( this->rect, obj.rect );
	SWAP<bool>( this->selected, obj.selected );
}

/*****************************
PQ MAP IMAGE
*****************************/

PQMapImage::PQMapImage(PQResourcePtr &resource)
	: PQMapNode(N_MAP_IMAGE)
{
	PQResImagePtr rImg = dynamic_pointer_cast<PQResImage>(resource);
	
	// cannot create a node with no resource
	assert( rImg );

	this->res = resource;
	this->row = rImg->newRow;
	this->col = rImg->newCol;
	this->angle = rImg->newAngle;
	this->scale = rImg->newScale;
	this->value1 = rImg->newValue1;
	this->value2 = rImg->newValue2;

	CalcRect();
}

PQMapImage::PQMapImage(const PQMapImage &that) : PQMapNode(that)
{
	this->row = that.row;
	this->col = that.col;
	this->angle = that.angle;
	this->scale = that.scale;
	this->value1 = that.value1;
	this->value2 = that.value2;
}

PQMapImage::~PQMapImage()
{

}

Undoable *PQMapImage::GetCopy()
{
	return new PQMapImage(*this);
}

void PQMapImage::Swap(Undoable *object)
{
	PQMapNode::Swap(object);

	PQMapImage &obj = *(PQMapImage*)object;

	SWAP<Uint8>( this->row, obj.row );
	SWAP<Uint8>( this->col, obj.col );
	SWAP<float>( this->angle, obj.angle );
	SWAP<float>( this->scale, obj.scale );
	SWAP<Int16>( this->value1, obj.value1 );
	SWAP<Int16>( this->value2, obj.value2 );
}

void PQMapImage::CalcRect()
{
	PQResImagePtr rImg = dynamic_pointer_cast<PQResImage>(res);

	if(rImg->img)
	{
		// adjust the rect based on this node's properties
		rect.w = (float)rImg->img->GetWidth() * scale;
		rect.h = (float)rImg->img->GetHeight() * scale;
		rect.x = position.x - (rect.w * 0.5f);
		rect.y = position.y - (rect.h * 0.5f);
	}

	return;
}

long PQMapImage::GetDataSize()
{
	return sizeof(Uint8) +		// type
		   sizeof(Uint8) +		// resIndex
		   sizeof(Vec2<float>) +	// position
		   sizeof(Uint8) +		// row
		   sizeof(Uint8) +		// col
		   sizeof(float) +		// angle
		   sizeof(float) +		// scale
		   sizeof(Int16) +		// value1
		   sizeof(Int16);		// value2
}

void PQMapImage::Draw(GL_Screen *screen, float offset_x, float offset_y, float shade, float alpha)
{
	CalcRect();
	res->Draw(screen, position.x + offset_x, position.y + offset_y, row, col, scale, angle, shade, alpha);
}

void PQMapImage::DrawThumb(GL_Screen *dest)
{
	if(res) res->DrawThumb(dest);
}

void PQMapImage::SetPosition(float x, float y)
{
	position.x = x;
	position.y = y;
	CalcRect();
}

void PQMapImage::SetPosition(const Vec2<float> &position)
{
	this->position = position;
	CalcRect();
}

void PQMapImage::Move(float offset_x, float offset_y)
{
	position.x += offset_x;
	position.y += offset_y;
	rect.x += offset_x;
	rect.y += offset_y;
}

PQMapNodePtr PQMapImage::Copy()
{
	return PQMapNodePtr( new PQMapImage(*this) );
}

/*****************************
PQ MAP SOUND
*****************************/
PQMapSound::PQMapSound(PQResourcePtr &resource)
	: PQMapNode(N_MAP_SOUND)
{
	PQResSoundPtr rSnd = dynamic_pointer_cast<PQResSound>(resource);
	
	// cannot create a node with no resource
	assert( rSnd );

	this->res = resource;
	this->volume = rSnd->newVolume;
	this->loop = rSnd->newLoop;
	this->delay = rSnd->newDelay;
	this->triggerRadius = rSnd->newTriggerRadius;

	CalcRect();
}

PQMapSound::PQMapSound(const PQMapSound &that) : PQMapNode(that)
{
	this->volume = that.volume;
	this->loop = that.loop;
	this->delay = that.delay;
	this->triggerRadius = that.triggerRadius;
}

PQMapSound::~PQMapSound()
{

}

Undoable *PQMapSound::GetCopy()
{
	return new PQMapSound(*this);
}

void PQMapSound::Swap(Undoable *object)
{
	PQMapNode::Swap(object);

	PQMapSound &obj = *(PQMapSound*)object;

	SWAP<float>( this->volume, obj.volume );
	SWAP<bool>( this->loop, obj.loop );
	SWAP<int>( this->delay, obj.delay );
	SWAP<int>( this->triggerRadius, obj.triggerRadius );
}

long PQMapSound::GetDataSize()
{
	return sizeof(Uint8) +			// type
		   sizeof(Uint8) +			// resIndex
		   sizeof(Vec2<float>) +	// position
		   sizeof(float) +			// volume
		   sizeof(bool) +			// loop
		   sizeof(int) +			// delay
		   sizeof(int);				// triggerRadius
}

void PQMapSound::Draw(GL_Screen *screen, float offset_x, float offset_y, float shade, float alpha)
{
	res->Draw(screen, position.x + offset_x, position.y + offset_y, 0, 0, 1.0f, 0, shade, alpha);

	if(triggerRadius > 0)
	{
		const Uint8 C = (Uint8)(shade * 255.0f);
		const Uint8 A = (Uint8)(alpha * 255.0f);
		GL_DrawCircle(screen, Vec2<float>(position.x + offset_x, position.y + offset_y), (float)triggerRadius, Color(0, C, 0, A), 2.0f);
	}

	
	CalcRect();
}

void PQMapSound::DrawThumb(GL_Screen *dest)
{
	if(res) res->DrawThumb(dest);
}

void PQMapSound::CalcRect()
{
	PQResSoundPtr rSnd = dynamic_pointer_cast<PQResSound>(res);

	// adjust the rect based on this node's properties
	rect.w = (float)rSnd->img->GetWidth();
	rect.h = (float)rSnd->img->GetHeight();
	rect.x = position.x - (rect.w / 2.0f);
	rect.y = position.y - (rect.h / 2.0f);
}

void PQMapSound::SetPosition(float x, float y)
{
	position.x = x;
	position.y = y;
	CalcRect();
}

void PQMapSound::SetPosition(const Vec2<float> &position)
{
	this->position = position;
	CalcRect();
}

void PQMapSound::Move(float offset_x, float offset_y)
{
	position.x += offset_x;
	position.y += offset_y;
	rect.x += offset_x;
	rect.y += offset_y;
}

PQMapNodePtr PQMapSound::Copy()
{
	return PQMapNodePtr( new PQMapSound(*this) );
}

/*****************************
PQ GRAPH NODE
*****************************/

PQGraphNode::PQGraphNode(float x, float y, Uint8 type)
	: PQMapNode(type), radius(6.0f)
{
	SetPosition(x, y);
	isDestination = false;
}

PQGraphNode::PQGraphNode(const PQGraphNode &that)
	: PQMapNode(that), radius(6.0f)
{
	this->neighbours = that.neighbours;
	this->isDestination = that.isDestination;
}

PQGraphNode::~PQGraphNode()
{
	
}

void PQGraphNode::Swap(Undoable *object)
{
	PQMapNode::Swap(object);

	PQGraphNode &obj = *(PQGraphNode*)object;

	SWAP< vector<PQGraphNodePtr> >( this->neighbours, obj.neighbours );
	SWAP<bool>(this->isDestination, obj.isDestination);
}

void PQGraphNode::TraceNeighbours(GL_Screen *dest, const Rect<float> &viewRect)
{
	PQGraphNodePtr node;

	for(GNodeIter it = neighbours.begin(); it != neighbours.end(); it++)
	{
		node = (*it);

		const Vec2<float> &p1 = position;
		const Vec2<float> &p2 = node->position;
		   
		if(viewRect.PointInside(p1.x, p1.y)
		|| viewRect.PointInside(p2.x, p2.y))
		{
			GL_DrawLine(dest, this->position, node->position, GREENCOLOR);
		}
	}
}

bool PQGraphNode::AddNeighbour(PQGraphNodePtr &neighbour)
{
	bool addNeighbour = true;

	for(size_t i = 0; i < neighbours.size(); i++)
	{
		if(neighbour == neighbours[i])
		{
			addNeighbour = false;
			break;
		}
	}

	if(addNeighbour)
		neighbours.push_back(neighbour);

	return addNeighbour;
}

bool PQGraphNode::RemoveNeighbour(PQGraphNodePtr &neighbour)
{
	bool removedNeighbour = false;

	for(size_t i = 0; i < neighbours.size(); i++)
	{
		if(neighbours[i] == neighbour)
		{
			neighbours.erase(neighbours.begin() + i);
			removedNeighbour = true;
			break;
		}
	}

	return removedNeighbour;
}

bool PQGraphNode::DisconnectAll()
{
	if(neighbours.size() > 0)
	{
		// go through own neighbours
		for(size_t i = 0; i < neighbours.size(); i++)
		{
			// check if neighbouur is connected to this node
			for(size_t j = 0; j < neighbours[i]->neighbours.size(); j++)
			{
				if(neighbours[i]->neighbours[j].get() == this)
				{
					// erase self from other node's neighbours
					neighbours[i]->neighbours.erase( neighbours[i]->neighbours.begin() + j);
					break;
				}
			}
		}

		neighbours.clear();

		return true;
	}

	return false;
}

bool PQGraphNode::DisconnectSelected()
{
	bool neighboursChanged = false;

	if(neighbours.size() > 0 && selected == true)
	{
		// go through own neighbours
		size_t i = 0;
		size_t numNeighbours = neighbours.size();
		bool nodeRemoved;
		
		while(i < numNeighbours)
		{
			nodeRemoved = false;

			if(neighbours[i]->selected == true)
			{
				// erase self from all selected neighbours
				for(size_t j = 0; j < neighbours[i]->neighbours.size(); j++)
				{
					if(neighbours[i]->neighbours[j].get() == this)
					{
						neighbours[i]->neighbours.erase( neighbours[i]->neighbours.begin() + j);
						break;
					}
				}

				neighbours.erase(neighbours.begin() + i);

				nodeRemoved = true;
			}

			if(nodeRemoved)
			{
				numNeighbours--;
				neighboursChanged = true;
			}
			else
			{
				i++;
			}
		}
	}

	return neighboursChanged;
}

long PQGraphNode::GetDataSize()
{
	return 0;
}

void PQGraphNode::CalcRect()
{
	rect.x = position.x - radius;
	rect.y = position.y - radius;
	rect.w = radius + radius;
	rect.h = radius + radius;
}

void PQGraphNode::SetPosition(float x, float y)
{
	position.set(x, y);
	CalcRect();
}

void PQGraphNode::Move(float offset_x, float offset_y)
{
	position.x += offset_x;
	position.y += offset_y;
	CalcRect();
}

/*****************************
PQ PEDESTRIAN GRAPH NODE
*****************************/

PQPedGraphNode::PQPedGraphNode(float x, float y)
	: PQGraphNode(x, y, N_PED_GRAPH_NODE)
{

}

PQPedGraphNode::PQPedGraphNode(const PQPedGraphNode &that) : PQGraphNode(that)
{

}

PQPedGraphNode::~PQPedGraphNode()
{

}

Undoable *PQPedGraphNode::GetCopy()
{
	return new PQPedGraphNode(*this);
}

void PQPedGraphNode::Swap(Undoable *object)
{
	PQGraphNode::Swap(object);
}

long PQPedGraphNode::GetDataSize()
{
	return 0;
}

void PQPedGraphNode::Draw(GL_Screen *dest, const Rect<float> &viewRect, float shade, float alpha)
{
	if( !viewRect.PointInside(position.x, position.y) )
		return;

	if(isDestination)
	{
		Color backColor(0, 162, 232);
		GL_FillCircle(dest, position, radius + 3, backColor);
	}


	Color clr;
	if(this->selected && alpha == 1.0f)
		clr.SetF(shade, shade, shade, alpha); // white
	else
		clr.SetF(shade, shade, 0.0f, alpha); // yellow

	GL_FillCircle(dest, position, radius, clr);
}

PQMapNodePtr PQPedGraphNode::Copy()
{
	return PQMapNodePtr( new PQPedGraphNode(*this) );
}

/*****************************
PQ VEHICLE GRAPH NODE
*****************************/

PQVehGraphNode::PQVehGraphNode(float x, float y)
	: PQGraphNode(x, y, N_VEH_GRAPH_NODE)
{

}

PQVehGraphNode::PQVehGraphNode(const PQVehGraphNode &that) : PQGraphNode(that)
{

}

PQVehGraphNode::~PQVehGraphNode()
{

}

Undoable *PQVehGraphNode::GetCopy()
{
	return new PQVehGraphNode(*this);
}

void PQVehGraphNode::Swap(Undoable *object)
{
	PQGraphNode::Swap(object);
}

long PQVehGraphNode::GetDataSize()
{
	return 0;
}

void PQVehGraphNode::Draw(GL_Screen *dest, const Rect<float> &viewRect, float shade, float alpha)
{
	if( !viewRect.PointInside(position.x, position.y) )
		return;

	if(isDestination)
	{
		Color backColor(0, 162, 232);
		GL_FillCircle(dest, position, radius + 3, backColor);
	}

	Color clr;
	if(this->selected && alpha == 1.0f)
		clr.SetF(shade, shade, shade, alpha); // white
	else
		clr.SetF(shade, 0.0f, 0.0f, alpha); // red

	GL_FillCircle(dest, position, radius, clr);
}

PQMapNodePtr PQVehGraphNode::Copy()
{
	return PQMapNodePtr( new PQVehGraphNode(*this) );
}