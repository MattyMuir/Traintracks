#include "Board.h"

Board::Board(int w_, int h_)
{
	Resize(w_, h_);
}

std::vector<Cell>& Board::operator[](int y)
{
	return cells[y];
}

const std::vector<Cell>& Board::operator[](int y) const
{
	return cells[y];
}

bool Board::In(int x, int y) const
{
	return (x >= 0 && x < w) && (y >= 0 && y < h);
}

bool Board::In(IntVec pos) const
{
	return In(pos.x, pos.y);
}

void Board::Resize(int w_, int h_)
{
	w = w_;
	h = h_;

	colLabels.resize(w);
	rowLabels.resize(h);

	ResetLabels();

	cells.resize(h);
	for (auto& row : cells)
		row.resize(w);
}

void Board::ResetLabels()
{
	for (auto& label : colLabels)
		label = 0;

	for (auto& label : rowLabels)
		label = 0;
}