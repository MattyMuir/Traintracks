#pragma once
#include <iostream>
#include <cassert>
#include <optional>

#include "tracks.h"
#include "Cell.h"

class Main
{
public:
	Main();
	void NewBoard();
	void NewLabels();

	std::vector<std::vector<Cell>> board;
	std::vector<int> colLabels, rowLabels;
	int gridW, gridH;
	bool unsolvable, display;

	bool Iterate();
	void Solve();
	int SolutionNum(std::vector<std::vector<Cell>>& boardIn);

private:

	//Helper Methods
	Direction GetOppositeDir(Direction dir);
	std::optional<Direction> NextTo(Vector2D first, Vector2D second);
	Vector2D DirToVec(Direction dir);
	Direction VecToDir(Vector2D dir);

	bool SetType(Vector2D pos, CellType newType);
	bool SetTrack(Vector2D pos, Direction dir, State newState);
	CellType ReadType(Vector2D pos);
	CellType ReadType(int x, int y);
	State ReadTrack(Vector2D pos, Direction dir);
	State ReadTrack(int x, int y, Direction dir);
	int StateCount(Vector2D pos, State s1, State s2);
	int StateCount(int x, int y, State s1, State s2);

	//Strategies
	bool BBlockedTrack();
	bool PRailHead();
	bool BFullRow();
	bool PFixedDir();
	bool PNSpaces();
	bool BDeadEnd();
	bool BParallelConnections();
	bool BImpossibleSide();
	bool BDoubleImpossibleSide();
	bool BClosedLoop();

	bool IsComplete();
};