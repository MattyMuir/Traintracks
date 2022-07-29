#pragma once
#include "tracks.h"

struct Cell
{
	Cell();

	CellType cellType;
	State tracks[4];
};