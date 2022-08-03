#include "Canvas.h"

wxBEGIN_EVENT_TABLE(Canvas, wxWindow)
	EVT_PAINT(Canvas::OnPaint)
	EVT_SIZE(Canvas::Resized)
	EVT_LEFT_DOWN(MouseDown)
	EVT_RIGHT_DOWN(MouseDownRight)
	EVT_RIGHT_UP(MouseUpRight)
	EVT_LEFT_UP(MouseUp)
	EVT_MOTION(MouseMoved)
	EVT_KEY_DOWN(OnKeyPressed)
wxEND_EVENT_TABLE()

Canvas::Canvas(wxWindow* parent) : wxWindow(parent, wxID_ANY),
	board(6, 6)
{
	SetCellWidth();
	remove = false;
	SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void Canvas::OnDraw(wxDC& dc)
{
	dc.Clear();

	DrawGrid(dc);
	DrawCells(dc);
	DrawLabels(dc);
}

void Canvas::OnPaint(wxPaintEvent&)
{
	wxBufferedPaintDC dc(this);
	wxGCDC gcdc(dc);

	GetSize(&screenW, &screenH);
	SetCellWidth();

	PrepareDC(gcdc);
	OnDraw(gcdc);
}

void Canvas::SetCellWidth()
{
	float cellSize = std::min((float)screenW / (board.w + 1), (float)screenH / (board.h + 1));
	cellW = cellSize;
	cellH = cellSize;
}

std::vector<std::string> Canvas::Split(std::string str, char delim)
{
	std::stringstream stream(str);
	std::string splitSingle;

	std::vector<std::string> splitStr;
	while (std::getline(stream, splitSingle, delim))
		splitStr.push_back(splitSingle);
	return splitStr;
}

std::vector<int> Canvas::StrVecToInt(const std::vector<std::string>& strVec)
{
	std::vector<int> result;
	for (std::string s : strVec)
		result.push_back(std::stoi(s, nullptr, 10));
	return result;
}

void Canvas::SetRailByIndex(IntVec pos, int railIndex, bool remove_)
{
	board.cells[pos.y][pos.x].cellType = CellType::RAIL;
	if (remove_)
	{
		board.cells[pos.y][pos.x].cellType = CellType::UNKNOWN;
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
			board.cells[pos.y][pos.x].cellType = CellType::BLOCKED;
			CASE(State::IMPOSSIBLE, State::IMPOSSIBLE, State::IMPOSSIBLE, State::IMPOSSIBLE);
			break;
		case 9:
			board.cells[pos.y][pos.x].cellType = CellType::UNKNOWN;
			CASE(State::POSSIBLE, State::POSSIBLE, State::POSSIBLE, State::POSSIBLE);
			break;
		}
	}
}

int Canvas::GetRailIndex(Cell cell)
{
	if (cell.tracks[(int)Direction::RIGHT] == State::GIVEN && cell.tracks[(int)Direction::LEFT] == State::GIVEN)
	{
		return 0;
	}
	if (cell.tracks[(int)Direction::UP] == State::GIVEN && cell.tracks[(int)Direction::DOWN] == State::GIVEN)
	{
		return 1;
	}
	if (cell.tracks[(int)Direction::UP] == State::GIVEN && cell.tracks[(int)Direction::RIGHT] == State::GIVEN)
	{
		return 2;
	}
	if (cell.tracks[(int)Direction::RIGHT] == State::GIVEN && cell.tracks[(int)Direction::DOWN] == State::GIVEN)
	{
		return 3;
	}
	if (cell.tracks[(int)Direction::DOWN] == State::GIVEN && cell.tracks[(int)Direction::LEFT] == State::GIVEN)
	{
		return 4;
	}
	if (cell.tracks[(int)Direction::LEFT] == State::GIVEN && cell.tracks[(int)Direction::UP] == State::GIVEN)
	{
		return 5;
	}
	return 9;
}

void Canvas::Open(std::string_view filename)
{
	std::ifstream read(filename.data());
	std::string line;

	std::getline(read, line);
	std::vector<std::string> dimStrVec = Split(line, ',');
	std::vector<int> dim = StrVecToInt(dimStrVec);

	board.Resize(dim[0], dim[1]);

	std::getline(read, line);
	std::vector<std::string> colStrVec = Split(line, ',');
	board.colLabels = StrVecToInt(colStrVec);

	std::getline(read, line);
	std::vector<std::string> rowStrVec = Split(line, ',');
	board.rowLabels = StrVecToInt(rowStrVec);

	for (int r = 0; r < board.h; r++)
	{
		std::getline(read, line);
		std::vector<std::string> cellIndexStr = Split(line, ',');
		std::vector<int> cellIndex = StrVecToInt(cellIndexStr);

		for (int c = 0; c < board.w; c++)
		{
			SetRailByIndex(IntVec(c, r), cellIndex[c], false);
		}
	}

	this->Refresh();
}

void Canvas::Save(std::string_view filename)
{
	std::ofstream write(filename.data());
	write << board.w << "," << board.h << "\n";

	for (int cl : board.colLabels)
		write << cl << ",";
	write << "\n";
	for (int cl : board.rowLabels)
		write << cl << ",";
	write << "\n";

	for (int r = 0; r < board.h; r++)
	{
		for (int c = 0; c < board.w; c++)
		{
			write << GetRailIndex(board.cells[r][c]) << ",";
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

	TGridW = cellW * board.w;
	TGridH = cellH * board.h;

	lineX = 0;
	lineY = screenH;

	for (int r = 0; r < board.h + 1; r++)
	{
		dc.GetGraphicsContext()->StrokeLine(0, lineY, TGridW, lineY);
		lineY -= cellH;
	}
	for (int c = 0; c < board.w + 1; c++)
	{
		dc.GetGraphicsContext()->StrokeLine(lineX, screenH - TGridH, lineX, screenH);
		lineX += cellW;
	}
}

IntVec Canvas::CellToScreen(IntVec pos)
{
	IntVec screen = IntVec();
	screen.x = pos.x * cellW + 1;
	screen.y = screenH - ((board.h - 1 - pos.y) * cellH + cellH) + 1;
	return screen;
}

IntVec Canvas::ScreenToCell(IntVec screen)
{
	IntVec pos = IntVec();
	pos.y = floor((float)(screen.y - (screenH - cellH * board.h)) / cellH);
	pos.x = floor((float)screen.x / cellW);

	return pos;
}

void Canvas::DrawCells(wxDC& dc)
{
	wxBrush brush = dc.GetBrush();
	wxPen pen = dc.GetPen();

	pen.SetWidth(2);

	for (int y = 0; y < board.h; y++)
	{
		for (int x = 0; x < board.w; x++)
		{
			Cell& currentCell = board[y][x];
			IntVec cellCorner = CellToScreen(IntVec(x, y));
			IntVec cellCenter = cellCorner;

			cellCenter.x += cellW / 2;
			cellCenter.y += cellH / 2;

			switch (currentCell.cellType)
			{
			case CellType::UNKNOWN:
				break;
			case CellType::RAIL:
				SetColour(pen, currentCell.tracks[(int)Direction::UP]);
				dc.SetPen(pen);
				dc.GetGraphicsContext()->StrokeLine(cellCenter.x, cellCenter.y, cellCenter.x, cellCenter.y - cellH / 2);
				SetColour(pen, currentCell.tracks[(int)Direction::DOWN]);
				dc.SetPen(pen);
				dc.GetGraphicsContext()->StrokeLine(cellCenter.x, cellCenter.y, cellCenter.x, cellCenter.y + cellH / 2);
				SetColour(pen, currentCell.tracks[(int)Direction::RIGHT]);
				dc.SetPen(pen);
				dc.GetGraphicsContext()->StrokeLine(cellCenter.x, cellCenter.y, cellCenter.x + cellW / 2, cellCenter.y);
				SetColour(pen, currentCell.tracks[(int)Direction::LEFT]);
				dc.SetPen(pen);
				dc.GetGraphicsContext()->StrokeLine(cellCenter.x, cellCenter.y, cellCenter.x - cellW / 2, cellCenter.y);
				break;
			case CellType::BLOCKED:
				pen.SetColour(*wxBLACK);
				pen.SetWidth(2);
				pen.SetStyle(wxPENSTYLE_SOLID);
				brush.SetColour(*wxBLACK);
				dc.SetPen(pen);
				dc.SetBrush(brush);
				dc.GetGraphicsContext()->DrawRectangle(cellCorner.x, cellCorner.y, cellW, cellH);
				break;
			}

		}
	}
}

void Canvas::DrawLabels(wxDC& dc)
{
	wxFont font = wxFont(0.5 * cellW, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_EXTRALIGHT);
	dc.SetFont(font);

	IntVec pos = ScreenToCell(IntVec(clickPosX, clickPosY));

	for (int c = 0; c < board.w; c++)
	{
		wxString labStr;
		labStr << (board.colLabels)[c];

		wxCoord width, height;
		dc.GetTextExtent(labStr, &width, &height);

		if (pos.y == -1 && pos.x == c)
		{
			dc.SetTextForeground(wxColour(255, 200, 50));
		}

		dc.GetGraphicsContext()->DrawText(labStr, c * cellW + cellW / 2 - width / 2, screenH - (cellH * board.h) - cellH / 2 - height / 2);
		dc.SetTextForeground(wxColour(0, 0, 0));
	}
	for (int r = 0; r < board.h; r++)
	{
		wxString labStr;
		labStr << (board.rowLabels)[r];

		wxCoord width, height;
		dc.GetTextExtent(labStr, &width, &height);

		if (pos.x == board.w && pos.y == r)
		{
			dc.SetTextForeground(wxColour(255, 200, 50));
		}

		dc.GetGraphicsContext()->DrawText(labStr, cellW * board.w + cellW / 2 - width / 2, screenH - (cellH * board.h) + cellH * r + cellH / 2 - height / 2);
		dc.SetTextForeground(wxColour(0, 0, 0));
	}
}

void Canvas::MouseDown(wxMouseEvent& evt)
{
	remove = false;
	clickPosX = evt.GetX();
	clickPosY = evt.GetY();
	IntVec screen = IntVec();
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
	IntVec screen = IntVec();
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
		IntVec screen = IntVec();
		screen.x = evt.GetX();
		screen.y = evt.GetY();

		PlaceRail(screen);
	}
	evt.Skip();
}

void Canvas::PlaceRail(IntVec screen)
{
	IntVec pos = ScreenToCell(screen);

	if (board.In(pos))
	{
		SetRailByIndex(pos, enterMode, remove);
		this->Refresh();
	}
}

void Canvas::OnKeyPressed(wxKeyEvent& evt)
{
	int enterModeBackup = enterMode;
	char c = evt.GetUnicodeKey();
	bool isNumeric = c - '0' >= 0 && c - '0' < 10;
	if (isNumeric)
		enterMode = c - '0';

	IntVec pos = ScreenToCell(IntVec(clickPosX, clickPosY));

	if (evt.GetKeyCode() == WXK_RETURN)
	{
		PuzzleSolver solver(&board);
		solver.Solve();
		Refresh();
	}
	else if (evt.GetKeyCode() == WXK_RIGHT)
	{
		PuzzleSolver solver(&board);
		solver.TakeStep();
		Refresh();
	}
	else if (evt.GetKeyCode() == 'N')
	{
		PuzzleSolver solver(&board);
		int solutionNum = solver.SolutionNum();
		Refresh();
		wxDialog* dlg = new wxDialog(this, wxID_ANY, "Result");
		wxString solutionNumText;
		solutionNumText << solutionNum;
		new wxStaticText(dlg, wxID_ANY, solutionNumText);

		if (dlg->ShowModal());
	}
	else
	{
		if (pos.y == -1 && pos.x < board.w)
		{
			if (isNumeric)
			{
				(board.colLabels)[pos.x] *= 10;
				(board.colLabels)[pos.x] += enterMode;
			}
			if (evt.GetKeyCode() == WXK_BACK) { (board.colLabels)[pos.x] = 0; }
			enterMode = enterModeBackup;
			this->Refresh();
		}
		if (pos.x == board.w && pos.y >= 0)
		{
			if (isNumeric)
			{
				(board.rowLabels)[pos.y] *= 10;
				(board.rowLabels)[pos.y] += enterMode;
			}
			if (evt.GetKeyCode() == WXK_BACK) { (board.rowLabels)[pos.y] = 0; }
			enterMode = enterModeBackup;
			this->Refresh();
		}
	}
}

void Canvas::Resized(wxSizeEvent& evt)
{
	int newW, newH;
	GetSize(&newW, &newH);
	Refresh();

	evt.Skip();
}