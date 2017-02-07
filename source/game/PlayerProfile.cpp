/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PlayerProfile.h"
#include "bytestream.h"
#include "utils.h"
#include <cassert>
#include <algorithm>

/////////////////////////////////
/////      LEVEL STATS     /////
/////////////////////////////////

LevelData::LevelData()
{
	ClearScore();
}

LevelData::LevelData(LevelData &&other)
{
	this->mapFilename			= move(other.mapFilename);
	this->songFilename			= move(other.songFilename);
	this->health				= other.health;
	this->deliveriesAssigned	= other.deliveriesAssigned;
	this->deliveriesCompleted	= other.deliveriesCompleted;
	this->timeLimit				= other.timeLimit;
	this->timeCompleted			= other.timeCompleted;
	this->score					= other.score;
	this->completed				= other.completed;

	other.health = 0;
	other.deliveriesAssigned = 0;
	other.deliveriesCompleted = 0;
	other.timeLimit = 0;
	other.timeCompleted = 0;
	other.score = 0;
	other.completed = false;
}

LevelData::LevelData(string mapFilename, string songFilename)
	: mapFilename(mapFilename), songFilename(songFilename)
{
	ClearScore();
}

LevelData::~LevelData()
{

}

void LevelData::ClearScore()
{
	health = 0;
	deliveriesAssigned = 0;
	deliveriesCompleted = 0;
	timeLimit = 0;
	timeCompleted = 0;
	score = 0;
	completed = false;
}

void LevelData::Evalutate()
{
// CHECK LEVEL COMPLETION
	completed = (health > 0
				&& deliveriesCompleted >= deliveriesAssigned
				&& timeCompleted < timeLimit);

// CALCULATE SCORE

	score = 0;

	// 2000 points for finishing with full health
	score += health * 2000 / 100;

	// 10 points per second less than the time limit
	score += (unsigned int)((timeLimit - timeCompleted) * 10.0f);

	// 500 points per delivery completed
	score += deliveriesCompleted * 500;
}

/////////////////////////////////
/////        PROFILE        /////
/////////////////////////////////

PlayerProfile::PlayerProfile()
{
	_currentLevel = 0;
}

PlayerProfile::~PlayerProfile()
{
	_currentLevel = 0;
}

void PlayerProfile::AddMap(string mapFilename,
						   string songFilename)
{
	that->levels.emplace_back(mapFilename, songFilename);
}

void PlayerProfile::ResetStats()
{
	for(auto& s : that->levels)
		s.ClearScore();

	that->_currentLevel = 0;
}

bool PlayerProfile::LoadProfile(const Path &path)
{
	if(!path.exists())
		return false;

	bytestream input = bytestream_from_file(path);

	if(input.empty())
		return false;

	size_t levelCount;
	input >> levelCount;
	input >> that->_currentLevel;

	that->levels.reserve(levelCount);

	for(size_t i = 0; i < levelCount; ++i)
	{
		LevelData level;

		input >> level.mapFilename;
		input >> level.songFilename;
		input >> level.health;
		input >> level.deliveriesAssigned;
		input >> level.deliveriesCompleted;
		input >> level.timeLimit;
		input >> level.timeCompleted;
		input >> level.score;
		input >> level.completed;

		that->levels.emplace_back(move(level));
	}

	return true;
}

bool PlayerProfile::SaveProfile(const Path &path)
{
	bytestream output;

	output << levelCount();
	output << that->_currentLevel;

	for(auto& level : that->levels)
	{
		output << level.mapFilename;
		output << level.songFilename;
		output << level.health;
		output << level.deliveriesAssigned;
		output << level.deliveriesCompleted;
		output << level.timeLimit;
		output << level.timeCompleted;
		output << level.score;
		output << level.completed;
	}

	return bytestream_to_file(path, output);
}

void PlayerProfile::AdvanceLevel()
{
	that->_currentLevel = min(that->_currentLevel + 1, that->levels.size() - 1);
}

bool PlayerProfile::DoneGame()
{
	return (that->_currentLevel == that->levels.size() - 1);
}

void PlayerProfile::SetCurrentLevel(size_t level)
{
	that->_currentLevel = min(level, that->levels.size() - 1);
}

size_t PlayerProfile::levelCount()
{
	return that->levels.size();
}

size_t PlayerProfile::currentLevel()
{
	return that->_currentLevel;
}

LevelData& PlayerProfile::GetLevelData(size_t level)
{
	assert(level < that->levels.size());
	return that->levels[level];
}

LevelData& PlayerProfile::currentLevelData()
{
	return that->levels[that->_currentLevel];
}
