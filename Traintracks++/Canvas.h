#pragma once
#include <wx/wx.h>

#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include "Board.h"
#include "PuzzleSolver.h"

#define CASE(up, down, right, left) \
	board.cells[pos.y][pos.x].tracks[(int)Direction::UP] = up; \
	board.cells[pos.y][pos.x].tracks[(int)Direction::DOWN] = down; \
	board.cells[pos.y][pos.x].tracks[(int)Direction::RIGHT] = right; \
	board.cells[pos.y][pos.x].tracks[(int)Direction::LEFT] = left

class Main;

class Canvas : public wxWindow
{
public:
	Canvas(wxWindow* parent);

	void Open(std::string_view filename);
	void Save(std::string_view filename);

private:
	int clickPosX, clickPosY;
	int enterMode;
	bool remove;

	int screenW, screenH;
	float cellW, cellH;

	Board board;

	void OnDraw(wxDC& dc);
	void OnPaint(wxPaintEvent&);

	void SetColour(wxPen& pen, State state);

	void SetCellWidth();

	std::vector<std::string> Split(std::string str, char delim);
	std::vector<int> StrVecToInt(const std::vector<std::string>& strVec);

	IntVec CellToScreen(IntVec pos);
	IntVec ScreenToCell(IntVec pos);

	void DrawGrid(wxDC& dc);
	void DrawCells(wxDC& dc);
	void DrawLabels(wxDC& dc);

	void MouseDown(wxMouseEvent& evt);
	void MouseDownRight(wxMouseEvent& evt);
	void MouseUp(wxMouseEvent& evt);
	void MouseUpRight(wxMouseEvent& evt);
	void MouseMoved(wxMouseEvent& evt);

	void SetRailByIndex(IntVec pos, int railIndex, bool remove_);
	int GetRailIndex(Cell cell);
	void PlaceRail(IntVec screen);

	void OnKeyPressed(wxKeyEvent& evt);
	void Resized(wxSizeEvent& evt);

	wxDECLARE_EVENT_TABLE();

	friend Main;
};