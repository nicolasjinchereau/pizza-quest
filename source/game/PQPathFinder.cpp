/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQPathFinder.h"
#include "PQGame.h"
#include <assert.h>
#include "Engine.h"

PQPathFinder::PQPathFinder(Graph *graph, vector<GraphNode*> *goals, float speed)
{
	this->speed = speed;
	pGraph = graph;
	pGoals = goals;
	following = false;
	minDistToNextGoal = 1000;
}

PQPathFinder::~PQPathFinder()
{

}

void PQPathFinder::Start()
{
	tween = AddChild(make_shared<MotionTween>());
	FollowPath();
}

void PQPathFinder::Update()
{
	if(following == true && tween->Empty() == true)
	{
		findPathRoutine = RunCoroutine([this](yield_token<float> yield)
		{
			following = false;

			yield(Random::range(4.0f, 30.0f));
			
			int tries = 0;

			while(!following && tries++ < 3)
			{
				following = FindPath();
				yield(0);
			}
		});
	}
}

void PQPathFinder::LateUpdate()
{
	if(following == true)
		position = tween->GetPosition();
}

void PQPathFinder::FollowPath()
{
	following = true;
}

void PQPathFinder::LeavePath()
{
	if(auto p = findPathRoutine.lock())
		CancelTask(findPathRoutine);

	following = false;
	tween->Clear();
}

bool PQPathFinder::FindPath()
{
	if(pGraph->Size() == 0 || pGoals->size() == 0)
		return false;

// get start of path
	GraphNode *start = pGraph->GetClosestNode(position);

// get next goal location
	vector<GraphNode*> possibleGoals;
	possibleGoals.reserve(pGoals->size() - 1);
	
// 
	for(auto goal : *pGoals)
	{
		if(position.Distance(goal->pos) >= minDistToNextGoal)
			possibleGoals.push_back(goal);
	}

	if(possibleGoals.empty())
		return false;

	auto it = pGoals->begin() + (rand() % (possibleGoals.size() - 1));
	GraphNode *end = *it;
	
	myPath.clear();

// find new path and initialize character
	if(pGraph->FindPath(start, end, myPath) && !myPath.empty())
	{
		tween->SetPath(position, myPath, speed);
		return true;
	}

	return false;
}

const vec2f &PQPathFinder::GetPosition()
{
	return tween->GetPosition();
}

const vec2f &PQPathFinder::GetDirection()
{
	return tween->GetDirection();
}