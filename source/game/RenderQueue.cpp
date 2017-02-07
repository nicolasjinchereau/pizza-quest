/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "RenderQueue.h"
#include "Object.h"

void RenderQueue::Submit(Object *pObject)
{
	that->_queue.push_back(pObject);
}

void RenderQueue::Clear()
{
	that->_queue.clear();
}

void RenderQueue::Sort()
{
	std::sort(that->_queue.begin(), that->_queue.end(), 
			[](const Object *x, const Object *y)
	{
		return x->layer < y->layer;
	});
}

void RenderQueue::Execute()
{
	for(Object *obj : that->_queue)
		obj->Draw();
}
