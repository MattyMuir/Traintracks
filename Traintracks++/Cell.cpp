#include "Cell.h"

Cell::Cell()
{
	cellType = CellType::UNKNOWN;
	tracks = { { "up", State::POSSIBLE },{ "down", State::POSSIBLE },{ "right", State::POSSIBLE },{ "left", State::POSSIBLE } };
}