#pragma once
#include <wx/wx.h>
#include "tracks.h"
#include "Main.h"

#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/dcbuffer.h"

#include <iostream>
#include <fstream>
#include <sstream>

#define CASE(up, down, right, left) \
	(*boardPtr)[pos.y][pos.x].tracks[(int)Direction::UP] = up; \
	(*boardPtr)[pos.y][pos.x].tracks[(int)Direction::DOWN] = down; \
	(*boardPtr)[pos.y][pos.x].tracks[(int)Direction::RIGHT] = right; \
	(*boardPtr)[pos.y][pos.x].tracks[(int)Direction::LEFT] = left

class Canvas : public wxWindow
{
public:
	Canvas(wxWindow* parent, 
		std::vector<std::vector<Cell>>* boardPtr_, std::vector<int>* colLabelsPtr_, std::vector<int>* rowLabelsPtr_, int* gridWPtr_, int* gridHPtr_);
	~Canvas();
	void m_Refresh(wxSizeEvent& evt);
	void m_Refresh();
	void SetCanvasDimension(int* gridWPtr_, int* gridHPtr_);
	void SetBoard(std::vector<std::vector<Cell>>* boardPtr_);
	void SetLabels(std::vector<int>* colLabelsPtr_, std::vector<int>* rowLabelsPtr_);

	void Open(std::string filename);
	void Save(std::string filename);

	Main* mainPtr;

private:
	int clickPosX, clickPosY;
	int enterMode;
	bool remove;

	int* gridWPtr,* gridHPtr;
	int screenW, screenH;
	float cellW, cellH;

	std::vector<std::vector<Cell>>* boardPtr;
	std::vector<int>* colLabelsPtr;
	std::vector<int>* rowLabelsPtr;

	void OnDraw(wxDC& dc);
	void OnPaint(wxPaintEvent& evt);

	void SetColour(wxPen& pen, State state);

	void SetCellWidth();

	std::vector<std::string> Split(std::string str, char delim);
	std::vector<int> StrVecToInt(std::vector<std::string> strVec);

	Vector2D CellToScreen(Vector2D pos);
	Vector2D ScreenToCell(Vector2D pos);

	void DrawGrid(wxDC& dc);
	void DrawCells(wxDC& dc);
	void DrawLabels(wxDC& dc);

	void MouseDown(wxMouseEvent& evt);
	void MouseDownRight(wxMouseEvent& evt);
	void MouseUp(wxMouseEvent& evt);
	void MouseUpRight(wxMouseEvent& evt);
	void MouseMoved(wxMouseEvent& evt);

	void SetRailByIndex(Vector2D pos, int railIndex, bool remove_);
	int GetRailIndex(Cell cell);
	void PlaceRail(Vector2D screen);

	void OnButtonPressed(wxKeyEvent& evt);

	wxDECLARE_EVENT_TABLE();
};