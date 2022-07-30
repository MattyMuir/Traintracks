#pragma once
#include <cassert>
#include <optional>

#include "Board.h"
#include "IntVec.h"
#include "Timer.h"

enum class Direction { UP = 0, DOWN, RIGHT, LEFT, NONE };
enum class StrategyResult { SUCCESS, UNSUITABLE, FOUND_FLAW };

class PuzzleSolver
{
public:
	PuzzleSolver(Board* boardPtr_);

	StrategyResult TakeStep();
	StrategyResult Solve();

protected:
	Board* boardPtr;

	// Helper methods
	static Direction Opposite(Direction dir);
	static std::optional<Direction> NextTo(IntVec first, IntVec second);
	static IntVec DirToVec(Direction dir);
	static Direction VecToDir(IntVec dir);

	StrategyResult SetType(int x, int y, CellType newType);
	StrategyResult SetType(IntVec pos, CellType newType);
	CellType ReadType(int x, int y) const;
	CellType ReadType(IntVec pos) const;
	StrategyResult SetTrack(int x, int y, Direction dir, State newState);
	StrategyResult SetTrack(IntVec pos, Direction dir, State newState);
	State ReadTrack(int x, int y, Direction dir) const;
	State ReadTrack(IntVec pos, Direction dir) const;
	int StateCount(int x, int y, State s1, State s2);
	int StateCount(IntVec pos, State s1, State s2);

	bool IsSolved() const;

	// Strategies
	StrategyResult BBlockedTrack();
	StrategyResult PRailHead();
	StrategyResult BFullRow();
	StrategyResult PFixedDir();
	StrategyResult PNSpaces();
	StrategyResult BDeadEnd();
	StrategyResult BParallelConnections();
	StrategyResult BImpossibleSide();
	StrategyResult BDoubleImpossibleSide();
	StrategyResult BClosedLoop();
};