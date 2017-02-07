/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include <fstream>
#include <string>
#include "Path.h"
#include "Singleton.h"
#include <vector>
#include <memory>
using namespace std;

/////////////////////////////////
/////      LEVEL RESULT     /////
/////////////////////////////////

class LevelData
{
public:
	LevelData();
	LevelData(LevelData &&other);
	LevelData(string mapFilename, string songFilename);
	~LevelData();

	void ClearScore();
	void Evalutate();

	string mapFilename;
	string songFilename;
	int health;
	int deliveriesAssigned;
	int deliveriesCompleted;
	float timeLimit;
	float timeCompleted;
	unsigned int score;
	bool completed;
};

class PlayerProfile : public Singleton<PlayerProfile>
{
	vector<LevelData> levels;
	size_t _currentLevel;

public:
	PlayerProfile();
	~PlayerProfile();
	
	static void AddMap(string mapFilename,
					   string songFilename);
	
	static bool LoadProfile(const Path &path);
	static bool SaveProfile(const Path &path);
	static void ResetStats();

	static void AdvanceLevel();
	static bool DoneGame();
	static void SetCurrentLevel(size_t level);
	static size_t levelCount();
	static size_t currentLevel();
	static LevelData& GetLevelData(size_t level);
	static LevelData& currentLevelData();
};