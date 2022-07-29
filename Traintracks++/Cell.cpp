#include "Cell.h"

Cell::Cell()
{
	cellType = CellType::UNKNOWN;
	for (int i = 0; i < 4; i++)
		tracks[i] = State::POSSIBLE;
}