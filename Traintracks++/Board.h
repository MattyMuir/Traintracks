#pragma once
#include <vector>

#include "Cell.h"
#include "IntVec.h"

class Canvas;
class PuzzleSolver;

class Board
{
public:
	Board() {};
	Board(int w_, int h_);

	std::vector<Cell>& operator[](int y);
	const std::vector<Cell>& operator[](int y) const;

	bool In(int x, int y) const;
	bool In(IntVec pos) const;

	void Resize(int w_, int h_);
	void ResetLabels();

	int w = 0, h = 0;

protected:
	std::vector<std::vector<Cell>> cells;
	std::vector<int> colLabels, rowLabels;

	friend Canvas;
	friend PuzzleSolver;
};