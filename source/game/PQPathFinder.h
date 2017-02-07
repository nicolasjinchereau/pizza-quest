/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "PQCharacter.h"
#include "Sprite.h"
#include "Animation.h"
#include "Trace.h"
#include <Box2D.h>
#include "MotionTween.h"
#include "Graph.h"
#include <list>
#include <deque>
#include "Object.h"
#include "PQGameTypes.h"

using namespace std;

class PQPathFinder : public PQCharacter
{
protected:
	shared_ptr<MotionTween> tween;
	deque<vec2f> myPath;
	Graph *pGraph;
	vector<GraphNode*> *pGoals;

	bool following;
	float speed;
	float minDistToNextGoal;
	weak_ptr<Task> findPathRoutine;

	bool FindPath();

public:

	PQPathFinder(Graph *graph, vector<GraphNode*> *goals, float speed);
	~PQPathFinder();

	virtual void Start() override;
	virtual void Update();
	virtual void LateUpdate();
	virtual string tag() const override { return "PathFinder"; }

	void FollowPath();
	void LeavePath();
	
	const vec2f &GetPosition();
	const vec2f &GetDirection();
};
