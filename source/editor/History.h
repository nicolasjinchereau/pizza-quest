/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <string>
#include <vector>
#include <cassert>
#include "list.h"

using namespace std;

template<class T>
void SWAP(T &a, T &b)
{
	T tmp = a;
	a = b;
	b = tmp;
}

class Undoable
{
public:
	virtual Undoable *GetCopy() = 0;
	virtual void Swap(Undoable *object) = 0;
	virtual ~Undoable(){}
};

class UndoState
{
public:

	UndoState(Undoable *object = NULL)
	{
		// cannot create an UndoState without an object
		assert(object);

		location = object;
		data = object->GetCopy();
	}

	virtual ~UndoState()
	{
		delete data;
	}

	void Swap()
	{
		location->Swap( data );
	}

	Undoable *location;
	Undoable *data;
};

struct Snapshot
{
	vector<UndoState*> objectStates;

	Snapshot(int numStates)
	{
		objectStates.reserve(numStates);
	}

	~Snapshot()
	{
		while(!objectStates.empty())
		{
			delete objectStates.back();
			objectStates.pop_back();
		}
	}

	void Swap()
	{
		for(int i = (int)objectStates.size() - 1; i >= 0; i--)
		{
			objectStates[i]->Swap();
		}
	}
};

class History
{
private:

	list<Snapshot*> history;
	list<Snapshot*> undone;
	Snapshot *snapshot;

	int historyLevels;

public:
	History(int historyLevels = 3)
	{
		this->historyLevels = historyLevels;
		snapshot = NULL;
	}

	~History()
	{
		Clear();
	}

	void Clear()
	{
		delete snapshot;
		snapshot = NULL;

		ClearUndo();
		ClearRedo();
	}

	void ClearUndo()
	{
		while(!history.empty())
		{
			delete history.back();
			history.pop_back();
		}
	}

	void ClearRedo()
	{
		while(!undone.empty())
		{
			delete undone.back();
			undone.pop_back();
		}
	}

	void SetHistoryLevels(int historyLevels)
	{
		this->historyLevels = historyLevels;

		while((int)history.size() > historyLevels)
		{
			delete history.back();
			history.pop_back();
		}
	}

	void BeginSnapshot(int numStates = 1)
	{
		// snapshot already in progress
		assert(snapshot == NULL);

		snapshot = new Snapshot( numStates );
	}

	void AddObjectState(Undoable *object)
	{
		// no snapshot currently in progress
		assert(snapshot);

		snapshot->objectStates.push_back( new UndoState(object) );
	}

	void EndSnapshot()
	{
		// call start snapshot first
		assert(snapshot);

		if(snapshot->objectStates.size() == 0)
		{
			delete snapshot;
		}
		else
		{
			history.push_front(snapshot);

			if((int)history.size() > historyLevels)
				history.pop_back();

			ClearRedo();
		}

		snapshot = NULL;
	}

	void CancelSnapshot()
	{
		delete snapshot;
		snapshot = NULL;
	}

	bool SnapshotOpen()
	{
		return snapshot != NULL;
	}

	void Undo()
	{
		if(history.empty())
		{
			return;
		}

		// SWAP stored state with current state
		history.front()->Swap();

		// move state undone list
		undone.push_front( history.front() );
		history.pop_front();
	}

	void Redo()
	{
		if(undone.empty())
		{
			return;
		}
		
		// SWAP stored state with current state
		undone.front()->Swap();

		history.push_front( undone.front() );
		undone.pop_front();
	}
};