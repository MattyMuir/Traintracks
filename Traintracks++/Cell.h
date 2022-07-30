#pragma once
enum class CellType { UNKNOWN, RAIL, BLOCKED };
enum class State { CERTAIN, POSSIBLE, IMPOSSIBLE, GIVEN };

struct Cell
{
	CellType cellType = CellType::UNKNOWN;
	State tracks[4] = { State::POSSIBLE, State::POSSIBLE, State::POSSIBLE, State::POSSIBLE };
};