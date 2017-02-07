/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "Graph.h"

void Graph::Node::AddNeighbour(Node *node)
{
	for(int i = 0; i < (int)neighbours.size(); i++)
    {
		if(neighbours[i] == node)
			return;
	}

	neighbours.push_back(node);
	distance.push_back(pos.Distance(node->pos));
}

void Graph::Node::RemoveNeighbour(Node *node)
{
	for(int i = 0; i < (int)neighbours.size(); i++)
	{
		if(neighbours[i] == node)
		{
			neighbours.erase(neighbours.begin() + i);
			return;
		}
	}
}

Graph::Graph()
{
		
}

Graph::~Graph()
{
	while(!nodes.empty())
	{
		delete nodes.back();
		nodes.pop_back();
	}
}

GraphNode *Graph::GetClosestNode(vec2f &pos)
{
	if(nodes.empty())
		return NULL;

	int idx = 0;
	float dist = nodes[0]->pos.Distance(pos);
	float tmp;

	for(int i = 1; i < (int)nodes.size(); i++)
	{
		tmp = nodes[i]->pos.Distance(pos);
		if(tmp < dist)
		{
			dist = tmp;
			idx = i;
		}
	}

	return nodes[idx];
}

void Graph::AddNode(float x, float y, bool isDestination)
{
	nodes.push_back(new Node(x, y, isDestination));
}

void Graph::RemoveNode(int n)
{
	Disconnect(n);

	delete nodes[n];
	nodes.erase(nodes.begin() + n);
}

void Graph::Connect(int a, int b)
{
	nodes[a]->AddNeighbour(nodes[b]);
	nodes[b]->AddNeighbour(nodes[a]);
}

void Graph::Disconnect(int n)
{
	for(size_t i = 0; i < nodes.size(); i++)
		nodes[i]->RemoveNeighbour(nodes[n]);
}
	
bool Graph::FindPath(Node *start, Node *finish, deque<vec2f> &path)
{
	open.clear();
	closed.clear();
	
	open.reserve(nodes.size());
	closed.reserve(nodes.size());
	
	// add the starting node record to the open list
	open.emplace_back(start,
					  nullptr,
					  0.0f,
					  0.0f + start->pos.Distance(finish->pos));

	while(!open.empty())
	{
		// find the node record with the lowest (cost + estimate)
		auto currentRecord = open.begin();
		
		for(auto it = currentRecord + 1, itEnd = open.end();
			it != itEnd; ++it)
		{
			if((*it).total < currentRecord->total)
				currentRecord = it;
		}
		
		// get a pointer to the current node
		Node *currentNode = currentRecord->node;

		// if the current node is the finish point
		if(currentNode == finish)
		{
			// add the finish node
			path.push_front(currentNode->pos);

			// add all the from nodes
			Node *from = currentRecord->from;

			while(!closed.empty())
			{
				// if this node record is where the path came from,
				if(closed.back().node == from) //&& closed.back()->from != NULL
				{
					// add it to the path
					path.push_front( from->pos );

					// get the next 'from' node
					from = closed.back().from;
				}

				// delete the node record
				closed.pop_back();
			}

			// add all the nodes to a queue and return it
			return true;
		}

		// cycle through all neighbours of the current node
		
		auto neighbour = currentNode->neighbours.begin();
		auto neigh_end = currentNode->neighbours.end();
		auto distance = currentNode->distance.begin();

		for( ; neighbour != neigh_end; ++neighbour, ++distance)
		{
			// check if neigbour is on the closed list
			bool isClosed = false;
			for(auto it = closed.begin(), it_end = closed.end();
				it != it_end;
				++it)
			{
				if(*neighbour == it->node)
				{
					isClosed = true;
					break;
				}
			}

			// skip if already on the closed list
			if(isClosed == true)
				continue;
			
			float cost = currentRecord->cost + *distance;
			float totalCost = cost + (*neighbour)->pos.Distance(finish->pos);
			
			// check if this neighbour is already on the open list
			bool isOpen = false;

			for(auto it = open.begin(), it_end = open.end();
				it != it_end; ++it)
			{
				if(*neighbour == it->node)
				{
					// node was found on the open list
					if(totalCost < it->total)
					{
						// node on open list was updated
						it->cost  = cost;
						it->total = totalCost;
						it->from  = currentNode;
					}

					isOpen = true;
					break;
				}
			}

			// skip if already on the open list
			if(isOpen == true)
				continue;

			// add to the open list
			open.emplace_back(*neighbour, currentNode, cost, totalCost);
		}

		// move the current node to the closed list after it has been evaluated
		closed.push_back(*currentRecord);
		open.erase(currentRecord);
	}

	// no path was found
	return false;
}

int Graph::Size()
{
	return (int)nodes.size();
}

GraphNode *Graph::operator[](int i) const
{
	return nodes[i];
}

