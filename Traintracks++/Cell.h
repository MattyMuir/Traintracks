#pragma once
#include "tracks.h"

class Cell
{
public:
	CellType cellType;
	std::unordered_map<std::string, State> tracks;

public:
	Cell();
};