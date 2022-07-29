#include "Canvas.h"

wxBEGIN_EVENT_TABLE(Canvas, wxWindow)
	EVT_PAINT(Canvas::OnPaint)
	EVT_SIZE(m_Refresh)
	EVT_LEFT_DOWN(MouseDown)
	EVT_RIGHT_DOWN(MouseDownRight)
	EVT_RIGHT_UP(MouseUpRight)
	EVT_LEFT_UP(MouseUp)
	EVT_MOTION(MouseMoved)
	EVT_KEY_DOWN(OnButtonPressed)
wxEND_EVENT_TABLE()

Canvas::Canvas(wxWindow* parent,
	std::vector<std::vector<Cell>>* boardPtr_,
				std::vector<int>* colLabelsPtr_,
				std::vector<int>* rowLabelsPtr_,
				int* gridWPtr_, int* gridHPtr_) : wxWindow(parent, wxID_ANY)
{
	boardPtr = boardPtr_;
	colLabelsPtr = colLabelsPtr_;
	rowLabelsPtr = rowLabelsPtr_;
	gridWPtr = gridWPtr_;
	gridHPtr = gridHPtr_;

	SetCellWidth();
	remove = false;
	SetBackgroundStyle(wxBG_STYLE_PAINT);
}

Canvas::~Canvas()
{

}

void Canvas::m_Refresh(wxSizeEvent& evt)
{
	this->Refresh();
}

void Canvas::m_Refresh()
{
	this->Refresh();
}

void Canvas::SetCanvasDimension(int* gridWPtr_, int* gridHPtr_)
{
	gridWPtr = gridWPtr_;
	gridHPtr = gridHPtr_;
}

void Canvas::SetBoard(std::vector<std::vector<Cell>>* boardPtr_)
{
	boardPtr = boardPtr_;
}

void Canvas::SetLabels(std::vector<int>* colLabelsPtr_, std::vector<int>* rowLabelsPtr_)
{
	colLabelsPtr = colLabelsPtr_;
	rowLabelsPtr = rowLabelsPtr_;
}

void Canvas::OnDraw(wxDC& dc)
{
	dc.Clear();
	DrawGrid(dc);
	DrawCells(dc);
	DrawLabels(dc);}

void Canvas::OnPaint(wxPaintEvent& evt)
{
	wxBufferedPaintDC dc(this);
	auto parent = this->GetParent();
	parent->GetSize(&screenW, &screenH);
	screenH -= 59;
	screenW -= 16;
	//dc.GetSize(&screenW, &screenH);
	SetCellWidth();

	this->PrepareDC(dc);
	this->OnDraw(dc);
}

void Canvas::SetCellWidth()
{
	cellW = (float)screenW / (*gridWPtr + 1);
	cellH = (float)screenH / (*gridHPtr + 1);

	if (cellW < cellH)
	{
		cellH = cellW;
	}
	else
	{
		cellW = cellH;
	}
}

std::vector<std::string> Canvas::Split(std::string str, char delim)
{
	std::vector<std::string> splitStr;
	std::istringstream stream(str);
	std::string splitSingle;

	while (std::getline(stream, splitSingle, delim))
	{
		splitStr.push_back(splitSingle);
	}
	return splitStr;
}

std::vector<int> Canvas::StrVecToInt(std::vector<std::string> strVec)
{
	std::vector<int> result;
	for (std::string s : strVec)
	{
		result.push_back(std::stoi(s, nullptr, 10));
	}
	return result;
}

void Canvas::SetRailByIndex(Vector2D pos, int railIndex, bool remove_)
{
	(*boardPtr)[pos.y][pos.x].cellType = CellType::RAIL;
	if (remove_)
	{
		(*boardPtr)[pos.y][pos.x].cellType = CellType::UNKNOWN;
		CASE(State::POSSIBLE, State::POSSIBLE, State::POSSIBLE, State::POSSIBLE);
	}
	else
	{
		switch (railIndex)
		{
		case 0:
			CASE(State::IMPOSSIBLE, State::IMPOSSIBLE, State::GIVEN, State::GIVEN);
			break;
		case 1:
			CASE(State::GIVEN, State::GIVEN, State::IMPOSSIBLE, State::IMPOSSIBLE);
			break;
		case 2:
			CASE(State::GIVEN, State::IMPOSSIBLE, State::GIVEN, State::IMPOSSIBLE);
			break;
		case 3:
			CASE(State::IMPOSSIBLE, State::GIVEN, State::GIVEN, State::IMPOSSIBLE);
			break;
		case 4:
			CASE(State::IMPOSSIBLE, State::GIVEN, State::IMPOSSIBLE, State::GIVEN);
			break;
		case 5:
			CASE(State::GIVEN, State::IMPOSSIBLE, State::IMPOSSIBLE, State::GIVEN);
			break;
		case 6:
			(*boardPtr)[pos.y][pos.x].cellType = CellType::BLOCKED;
			CASE(State::IMPOSSIBLE, State::IMPOSSIBLE, State::IMPOSSIBLE, State::IMPOSSIBLE);
			break;
		case 9:
			(*boardPtr)[pos.y][pos.x].cellType = CellType::UNKNOWN;
			break;
		}
	}
}

int Canvas::GetRailIndex(Cell cell)
{
	if (cell.tracks["right"] == State::GIVEN && cell.tracks["left"] == State::GIVEN)
	{
		return 0;
	}
	if (cell.tracks["up"] == State::GIVEN && cell.tracks["down"] == State::GIVEN)
	{
		return 1;
	}
	if (cell.tracks["up"] == State::GIVEN && cell.tracks["right"] == State::GIVEN)
	{
		return 2;
	}
	if (cell.tracks["right"] == State::GIVEN && cell.tracks["down"] == State::GIVEN)
	{
		return 3;
	}
	if (cell.tracks["down"] == State::GIVEN && cell.tracks["left"] == State::GIVEN)
	{
		return 4;
	}
	if (cell.tracks["left"] == State::GIVEN && cell.tracks["up"] == State::GIVEN)
	{
		return 5;
	}
	return 9;
}

void Canvas::Open(std::string filename)
{
	std::ifstream read(filename);
	std::string line;

	std::getline(read, line);
	std::vector<std::string> dimStrVec = Split(line, ',');
	std::vector<int> dim = StrVecToInt(dimStrVec);
	*gridWPtr = dim[0];
	*gridHPtr = dim[1];

	std::getline(read, line);
	std::vector<std::string> colStrVec = Split(line, ',');
	*colLabelsPtr = StrVecToInt(colStrVec);

	std::getline(read, line);
	std::vector<std::string> rowStrVec = Split(line, ',');
	*rowLabelsPtr = StrVecToInt(rowStrVec);

	mainPtr->NewBoard();

	for (int r = 0; r < *gridHPtr; r++)
	{
		std::getline(read, line);
		std::vector<std::string> cellIndexStr = Split(line, ',');
		std::vector<int> cellIndex = StrVecToInt(cellIndexStr);

		for (int c = 0; c < *gridWPtr; c++)
		{
			SetRailByIndex(Vector2D(c, r), cellIndex[c], false);
		}
	}

	this->Refresh();
}

void Canvas::Save(std::string filename)
{
	std::ofstream write(filename);
	write << (*gridWPtr) << "," << (*gridHPtr) << "\n";

	for (int cl : *colLabelsPtr)
		write << cl << ",";
	write << "\n";
	for (int cl : *rowLabelsPtr)
		write << cl << ",";
	write << "\n";

	for (int r = 0; r < (*gridHPtr); r++)
	{
		for (int c = 0; c < (*gridWPtr); c++)
		{
			write << GetRailIndex((*boardPtr)[r][c]) << ",";
		}
		write << "\n";
	}

	write.close();
}

void Canvas::SetColour(wxPen& pen, State state)
{
	pen.SetStyle(wxPENSTYLE_SOLID);
	switch (state)
	{
	case State::CERTAIN:
		pen.SetColour(wxColour(0, 0, 255));
		break;
	case State::POSSIBLE:
		pen.SetColour(wxColour(255, 0, 0));
		break;
	case State::IMPOSSIBLE:
		pen.SetStyle(wxPENSTYLE_TRANSPARENT);
		pen.SetColour(wxTransparentColour);
		break;
	case State::GIVEN:
		pen.SetColour(wxColour(0, 0, 0));
		break;
	}
}

void Canvas::DrawGrid(wxDC& dc)
{
	wxBrush brush = dc.GetBrush();
	wxPen pen = dc.GetPen();

	pen.SetStyle(wxPENSTYLE_SOLID);
	pen.SetColour(*wxBLACK);

	dc.SetPen(pen);
	dc.SetBrush(brush);
	
	float TGridW, TGridH, lineX, lineY;

	TGridW = cellW * (*gridWPtr);
	TGridH = cellH * (*gridHPtr);

	lineX = 0;
	lineY = screenH;

	for (int r = 0; r < (*gridHPtr) + 1; r++)
	{
		dc.DrawLine(0, lineY, TGridW, lineY);
		lineY -= cellH;
	}
	for (int c = 0; c < (*gridWPtr) + 1; c++)
	{
		dc.DrawLine(lineX, screenH - TGridH, lineX, screenH);
		lineX += cellW;
	}
}

Vector2D Canvas::CellToScreen(Vector2D pos)
{
	Vector2D screen = Vector2D();
	screen.x = pos.x * cellW + 1;
	screen.y = screenH - (((*gridHPtr) - 1 - pos.y) * cellH + cellH) + 1;
	return screen;
}

Vector2D Canvas::ScreenToCell(Vector2D screen)
{
	Vector2D pos = Vector2D();
	pos.y = floor((float)(screen.y - (screenH - cellH * (*gridHPtr))) / cellH);
	pos.x = floor((float)screen.x / cellW);

	return pos;
}

void Canvas::DrawCells(wxDC& dc)
{
	//Create reference for readability
	std::vector<std::vector<Cell>>& board = *boardPtr;

	wxBrush brush = dc.GetBrush();
	wxPen pen = dc.GetPen();

	pen.SetWidth(2);

	for (int y = 0; y < (*gridHPtr); y++)
	{
		for (int x = 0; x < (*gridWPtr); x++)
		{
			Cell& currentCell = board[y][x];
			Vector2D cellCorner = CellToScreen(Vector2D(x, y));
			Vector2D cellCenter = cellCorner;

			cellCenter.x += cellW / 2;
			cellCenter.y += cellH / 2;

			switch (currentCell.cellType)
			{
			case CellType::UNKNOWN:
				break;
			case CellType::RAIL:
				SetColour(pen, currentCell.tracks["up"]);
				dc.SetPen(pen);
				dc.DrawLine(cellCenter.x, cellCenter.y, cellCenter.x, cellCenter.y - cellH / 2);
				SetColour(pen, currentCell.tracks["down"]);
				dc.SetPen(pen);
				dc.DrawLine(cellCenter.x, cellCenter.y, cellCenter.x, cellCenter.y + cellH / 2);
				SetColour(pen, currentCell.tracks["right"]);
				dc.SetPen(pen);
				dc.DrawLine(cellCenter.x, cellCenter.y, cellCenter.x + cellW / 2, cellCenter.y);
				SetColour(pen, currentCell.tracks["left"]);
				dc.SetPen(pen);
				dc.DrawLine(cellCenter.x, cellCenter.y, cellCenter.x - cellW / 2, cellCenter.y);
				break;
			case CellType::BLOCKED:
				pen.SetColour(*wxBLACK);
				pen.SetWidth(2);
				pen.SetStyle(wxPENSTYLE_SOLID);
				brush.SetColour(*wxBLACK);
				dc.SetPen(pen);
				dc.SetBrush(brush);
				dc.DrawRectangle(cellCorner.x, cellCorner.y, cellW, cellH);
				break;
			}

		}
	}
}

void Canvas::DrawLabels(wxDC& dc)
{
	wxFont font = wxFont(0.5 * cellW, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_EXTRALIGHT);
	dc.SetFont(font);

	Vector2D pos = ScreenToCell(Vector2D(clickPosX, clickPosY));

	for (int c = 0; c < (*gridWPtr); c++)
	{
		wxString labStr;
		labStr << (*colLabelsPtr)[c];

		wxCoord width, height;
		dc.GetTextExtent(labStr, &width, &height);

		if (pos.y == -1 && pos.x == c)
		{
			dc.SetTextForeground(wxColour(255, 200, 50));
		}

		dc.DrawText(labStr, c * cellW + cellW / 2 - width / 2, screenH - (cellH * (*gridHPtr)) - cellH / 2 - height / 2);
		dc.SetTextForeground(wxColour(0, 0, 0));
	}
	for (int r = 0; r < (*gridHPtr); r++)
	{
		wxString labStr;
		labStr << (*rowLabelsPtr)[r];

		wxCoord width, height;
		dc.GetTextExtent(labStr, &width, &height);

		if (pos.x == *gridWPtr && pos.y == r)
		{
			dc.SetTextForeground(wxColour(255, 200, 50));
		}

		dc.DrawText(labStr, cellW * (*gridWPtr) + cellW / 2 - width / 2, screenH - (cellH * (*gridHPtr)) + cellH * r + cellH / 2 - height / 2);
		dc.SetTextForeground(wxColour(0, 0, 0));
	}
}

void Canvas::MouseDown(wxMouseEvent& evt)
{
	remove = false;
	clickPosX = evt.GetX();
	clickPosY = evt.GetY();
	Vector2D screen = Vector2D();
	screen.x = evt.GetX();
	screen.y = evt.GetY();
	PlaceRail(screen);

	this->Refresh();
	evt.Skip();
}

void Canvas::MouseDownRight(wxMouseEvent& evt)
{
	clickPosX = evt.GetX();
	clickPosY = evt.GetY();
	Vector2D screen = Vector2D();
	screen.x = evt.GetX();
	screen.y = evt.GetY();

	remove = true;
	PlaceRail(screen);
	evt.Skip();
}

void Canvas::MouseUp(wxMouseEvent& evt)
{
	evt.Skip();
}

void Canvas::MouseUpRight(wxMouseEvent& evt)
{
	remove = false;
	evt.Skip();
}

void Canvas::MouseMoved(wxMouseEvent& evt)
{
	if (wxGetMouseState().LeftIsDown() || wxGetMouseState().RightIsDown())
	{
		Vector2D screen = Vector2D();
		screen.x = evt.GetX();
		screen.y = evt.GetY();

		PlaceRail(screen);
	}
	evt.Skip();
}

void Canvas::PlaceRail(Vector2D screen)
{
	Vector2D pos = ScreenToCell(screen);

	int gridW = *gridWPtr;
	int gridH = *gridHPtr;
	if (InsideGrid(pos))
	{
		SetRailByIndex(pos, enterMode, remove);
		this->Refresh();
	}
}

void Canvas::OnButtonPressed(wxKeyEvent& evt)
{
	int enterModeBackup = enterMode;
	char c = evt.GetUnicodeKey();
	bool isNumeric = c - '0' >= 0 && c - '0' < 10;
	if (isNumeric)
	{
		enterMode = c - '0';
	}

	Vector2D pos = ScreenToCell(Vector2D(clickPosX, clickPosY));

	if (evt.GetKeyCode() == WXK_RETURN)
	{
		mainPtr->Solve();
		this->Refresh();
	}
	else if (evt.GetKeyCode() == WXK_RIGHT)
	{
		mainPtr->Iterate();
		this->Refresh();
	}
	else if (evt.GetKeyCode() == 'N')
	{
		std::ofstream cout = std::ofstream("C:\\Users\\matty\\Desktop\\out.txt");
		cout << mainPtr->SolutionNum(*boardPtr);
		this->Refresh();
		cout.close();
	}
	else
	{
		if (pos.y == -1 && pos.x < (*gridWPtr))
		{
			if (isNumeric)
			{
				(*colLabelsPtr)[pos.x] *= 10;
				(*colLabelsPtr)[pos.x] += enterMode;
			}
			if (evt.GetKeyCode() == WXK_BACK) { (*colLabelsPtr)[pos.x] = 0; }
			enterMode = enterModeBackup;
			this->Refresh();
		}
		if (pos.x == (*gridWPtr) && pos.y >= 0)
		{
			if (isNumeric)
			{
				(*rowLabelsPtr)[pos.y] *= 10;
				(*rowLabelsPtr)[pos.y] += enterMode;
			}
			if (evt.GetKeyCode() == WXK_BACK) { (*rowLabelsPtr)[pos.y] = 0; }
			enterMode = enterModeBackup;
			this->Refresh();
		}
	}
}