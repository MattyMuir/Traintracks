#pragma once
#include "tracks.h"
#include "Cell.h"

#include <iostream>
#include <fstream>

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
	std::ofstream cout;

	//Helper Methods
	std::string GetOppositeDir(std::string dir);
	bool NextTo(std::string& dir, Vector2D first, Vector2D second);
	Vector2D DirVal(std::string dir);

	bool SetType(Vector2D pos, CellType newType);
	bool SetTrack(Vector2D pos, std::string dir, State newState);
	CellType ReadType(Vector2D pos);
	State ReadTrack(Vector2D pos, std::string dir);
	int StateCount(Vector2D pos, State s1, State s2);

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