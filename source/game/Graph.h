/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include <stdlib.h>
#include <list>
#include <vector>
#include <deque>
#include "Math.h"

using namespace std;

class Graph
{
public:
	
	struct Node
	{
		vec2f pos;
		bool isDestination;

		vector<Node*> neighbours;
		vector<float> distance;
		
		Node(float x, float y, bool isDestination) : pos(x, y), isDestination(isDestination){}

		void AddNeighbour(Node *node);
		void RemoveNeighbour(Node *node);
	};

	struct NodeRecord
	{
		NodeRecord(Node *node = nullptr,
				   Node *from = nullptr,
				   float cost = 0.0f,
				   float total = 0.0f)
			: node(node),
			  from(from),
			  cost(cost),
			  total(total){}

		Node *node;
		Node *from;
		float cost; // cost so far
		float total; // cost + estimate
	};

	vector<Node*> nodes;
	vector<NodeRecord> open;
	vector<NodeRecord> closed;

	Graph();
	~Graph();

	Node *GetClosestNode(vec2f &pos);
	void AddNode(float x, float y, bool isDestination);
	void RemoveNode(int n);
	void Connect(int a, int b);
	void Disconnect(int n);
	bool FindPath(Node *start, Node *finish, deque<vec2f> &path);
	int Size();
	Node *operator[](int i) const;
};

typedef Graph::Node GraphNode;
