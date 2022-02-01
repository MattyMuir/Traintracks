#include "cMain.h"

cMain::cMain()
{
	cout = std::ofstream("C:\\Users\\matty\\Desktop\\railslog.txt");

	gridW = 6;
	gridH = 6;

	NewBoard();
	NewLabels();

	unsolvable = false;
	display = true;
}

void cMain::NewBoard()
{
	board.clear();
	for (int y = 0; y < gridH; y++)
	{
		board.push_back(std::vector<Cell>());
		for (int x = 0; x < gridW; x++)
		{
			board[y].push_back(Cell());
		}
	}
}

void cMain::NewLabels()
{
	colLabels.clear();
	rowLabels.clear();
	for (int c = 0; c < gridW; c++)
	{
		colLabels.push_back(0);
	}
	for (int r = 0; r < gridH; r++)
	{
		rowLabels.push_back(0);
	}
}

bool cMain::Iterate()
{
	bool done = false;

	if (!done) { done = PRailHead(); }
	if (!done) { done = BBlockedTrack(); }
	if (!done) { done = BFullRow(); }
	if (!done) { done = PFixedDir(); }
	if (!done) { done = PNSpaces(); }
	if (!done) { done = BDeadEnd(); }
	if (!done) { done = BParallelConnections(); }
	if (!done) { done = BImpossibleSide(); }
	if (!done) { done = BDoubleImpossibleSide(); }
	if (!done) { done = BClosedLoop(); }
	return done;
}

void cMain::Solve()
{
	cout << "hey";
	bool done = false;

	while (!done && !unsolvable)
	{
		done = Iterate();
		done = !done;
	}
}

std::string cMain::GetOppositeDir(std::string dir)
{
	if (dir == "up") { return "down"; }
	if (dir == "down") { return "up"; }
	if (dir == "right") { return "left"; }
	if (dir == "left") { return "right"; }
	throw;
}

bool cMain::NextTo(std::string& dir, Vector2D first, Vector2D second)
{
	bool nextTo = false;
	if (first.x - second.x == 1 && first.y == second.y) { dir = "left"; nextTo = true; }
	if (first.x - second.x == -1 && first.y == second.y) { dir = "right"; nextTo = true; }
	if (first.y - second.y == 1 && first.x == second.x) { dir = "up"; nextTo = true; }
	if (first.y - second.y == -1 && first.x == second.x) { dir = "down"; nextTo = true;}

	return nextTo;
}

Vector2D cMain::DirVal(std::string dir)
{
	if (dir == "up") { return Vector2D(0, -1); }
	if (dir == "down") { return Vector2D(0, 1); }
	if (dir == "right") { return Vector2D(1, 0); }
	if (dir == "left") { return Vector2D(-1, 0); }
	throw;
}

bool cMain::SetType(Vector2D pos, CellType newType)
{
	bool applicable = false;
	if (InsideGrid(pos))
	{
		applicable = (board[pos.y][pos.x].cellType != newType);
		if (board[pos.y][pos.x].cellType == CellType::BLOCKED && newType == CellType::RAIL)
		{
			//throw;
			unsolvable = true;
			//cout << "Type set: "<< pos.x << "," << pos.y << std::endl;
			return false;
		}
		if (board[pos.y][pos.x].cellType == CellType::RAIL && newType == CellType::BLOCKED)
		{
			//throw;
			unsolvable = true;
			//cout << "Type set: " << pos.x << "," << pos.y << std::endl;
			return false;
		}

		board[pos.y][pos.x].cellType = newType;

		if (newType == CellType::BLOCKED)
		{
			std::unordered_map<std::string, State>::iterator it;
			for (it = board[pos.y][pos.x].tracks.begin(); it != board[pos.y][pos.x].tracks.end(); it++)
			{
				it->second = State::IMPOSSIBLE;
			}
		}
	}
	else
	{
		return false;
	}
	return applicable;
}

bool cMain::SetTrack(Vector2D pos, std::string dir, State newState)
{
	bool applicable = false;
	if (InsideGrid(pos))
	{
		State& currentState = board[pos.y][pos.x].tracks[dir];
		if (currentState == State::GIVEN) { return false; }
		if (currentState == State::CERTAIN && newState == State::IMPOSSIBLE)
		{
			//throw;
			unsolvable = true;
			//cout << "Track set: " << pos.x << "," << pos.y << " Dir: " << dir << std::endl;
			return false;
		}
		if (currentState == State::IMPOSSIBLE && newState == State::CERTAIN)
		{
			//throw;
			unsolvable = true;
			//cout << "Track set: " << pos.x << "," << pos.y << " Dir: " << dir << std::endl;
			return false;
		}
		applicable = (currentState != newState);
		currentState = newState;

		if (board[pos.y][pos.x].cellType == CellType::UNKNOWN) { applicable = false; }
	}
	else
	{
		return false;
	}
	return applicable;
}

CellType cMain::ReadType(Vector2D pos)
{
	if (InsideGrid(pos))
	{
		return board[pos.y][pos.x].cellType;
	}
	return CellType::BLOCKED;
}

State cMain::ReadTrack(Vector2D pos, std::string dir)
{
	if (InsideGrid(pos))
	{
		return board[pos.y][pos.x].tracks[dir];
	}
	return State::IMPOSSIBLE;
}

int cMain::StateCount(Vector2D pos, State s1, State s2)
{
	int count = 0;
	std::unordered_map<std::string, State>::iterator it;
	for (it = board[pos.y][pos.x].tracks.begin(); it != board[pos.y][pos.x].tracks.end(); it++)
	{
		if (it->second == s1 || it->second == s2)
		{
			count++;
		}
	}
	return count;
}

bool cMain::BBlockedTrack()
{
	bool applicable = false;
	bool setTrack;
	for (int r = 0; r < gridH; r++)
	{
		for (int c = 0; c < gridW; c++)
		{
			std::unordered_map<std::string, State>::iterator it;
			for (it = board[r][c].tracks.begin(); it != board[r][c].tracks.end(); it++)
			{
				Vector2D nextPos = Vector2D(c, r);
				nextPos += DirVal(it->first);
				if (ReadTrack(nextPos, GetOppositeDir(it->first)) == State::IMPOSSIBLE)
				{
					setTrack = SetTrack(Vector2D(c, r), it->first, State::IMPOSSIBLE);
					applicable = applicable || setTrack;
				}
			}
		}
	}
	return applicable;
}

bool cMain::PRailHead()
{
	bool applicable = false;
	bool setType, setTrack;
	for (int r = 0; r < gridH; r++)
	{
		for (int c = 0; c < gridW; c++)
		{
			if (ReadType(Vector2D(c, r)) == CellType::UNKNOWN || ReadType(Vector2D(c, r)) == CellType::BLOCKED)
			{
				continue;
			}

			std::unordered_map<std::string, State>::iterator it;
			for (it = board[r][c].tracks.begin(); it != board[r][c].tracks.end(); it++)
			{
				if (it->second == State::CERTAIN || it->second == State::GIVEN)
				{
					Vector2D nextPos = Vector2D(c, r);
					nextPos += DirVal(it->first);
					setType = SetType(nextPos, CellType::RAIL);
					setTrack = SetTrack(nextPos, GetOppositeDir(it->first), State::CERTAIN);
					applicable = applicable || setType || setTrack;
				}
			}
		}
	}
	return applicable;
}

bool cMain::BFullRow()
{
	bool applicable = false;
	bool blocked;

	int railNum = 0;
	for (int c = 0; c < gridW; c++)
	{
		railNum = 0;
		for (int r = 0; r < gridH; r++)
		{
			if (ReadType(Vector2D(c, r)) == CellType::RAIL) { railNum++; }
		}
		if (railNum > colLabels[c]) { unsolvable = true; /*throw; cout << "Col: " << c;*/ }
		if (railNum == colLabels[c])
		{
			for (int r = 0; r < gridH; r++)
			{
				if (ReadType(Vector2D(c, r)) == CellType::UNKNOWN)
				{
					blocked = SetType(Vector2D(c, r), CellType::BLOCKED);
					applicable = applicable || blocked;
				}
			}
		}
	}
	for (int r = 0; r < gridH; r++)
	{
		railNum = 0;
		for (int c = 0; c < gridW; c++)
		{
			if (ReadType(Vector2D(c, r)) == CellType::RAIL) { railNum++; }
		}
		if (railNum > rowLabels[r]) { unsolvable = true; /*throw; cout << "Row: " << r;*/ }
		if (railNum == rowLabels[r])
		{
			for (int c = 0; c < gridW; c++)
			{
				if (ReadType(Vector2D(c, r)) == CellType::UNKNOWN)
				{
					blocked = SetType(Vector2D(c, r), CellType::BLOCKED);
					applicable = applicable || blocked;
				}
			}
		}
	}
	return applicable;
}

bool cMain::PFixedDir()
{
	bool applicable = false;
	bool setTrack;
	for (int r = 0; r < gridH; r++)
	{
		for (int c = 0; c < gridW; c++)
		{
			if (ReadType(Vector2D(c, r)) != CellType::RAIL) { continue; }
			if (StateCount(Vector2D(c, r), State::IMPOSSIBLE, State::IMPOSSIBLE) == 2)
			{
				std::unordered_map<std::string, State>::iterator it;
				for (it = board[r][c].tracks.begin(); it != board[r][c].tracks.end(); it++)
				{
					if (it->second == State::POSSIBLE)
					{
						setTrack = SetTrack(Vector2D(c, r), it->first, State::CERTAIN);
						applicable = applicable || setTrack;
					}
				}
			}
			if (StateCount(Vector2D(c, r), State::CERTAIN, State::CERTAIN) == 2)
			{
				std::unordered_map<std::string, State>::iterator it;
				for (it = board[r][c].tracks.begin(); it != board[r][c].tracks.end(); it++)
				{
					if (it->second == State::POSSIBLE)
					{
						setTrack = SetTrack(Vector2D(c, r), it->first, State::IMPOSSIBLE);
						applicable = applicable || setTrack;
					}
				}
			}
		}
	}
	return applicable;
}

bool cMain::PNSpaces()
{
	bool applicable = false;
	bool placed;

	int blockedNum = 0;
	for (int c = 0; c < gridW; c++)
	{
		blockedNum = 0;
		for (int r = 0; r < gridH; r++)
		{
			if (ReadType(Vector2D(c, r)) == CellType::BLOCKED) { blockedNum++; }
		}
		if (blockedNum == gridH - colLabels[c])
		{
			for (int r = 0; r < gridH; r++)
			{
				if (ReadType(Vector2D(c, r)) == CellType::UNKNOWN)
				{
					placed = SetType(Vector2D(c, r), CellType::RAIL);
					applicable = applicable || placed;
				}
			}
		}
	}
	for (int r = 0; r < gridH; r++)
	{
		blockedNum = 0;
		for (int c = 0; c < gridW; c++)
		{
			if (ReadType(Vector2D(c, r)) == CellType::BLOCKED) { blockedNum++; }
		}
		if (blockedNum == gridW - rowLabels[r])
		{
			for (int c = 0; c < gridW; c++)
			{
				if (ReadType(Vector2D(c, r)) == CellType::UNKNOWN)
				{
					placed = SetType(Vector2D(c, r), CellType::RAIL);
					applicable = applicable || placed;
				}
			}
		}
	}
	return applicable;
}

bool cMain::BDeadEnd()
{
	bool applicable = false;
	bool blocked;
	for (int r = 0; r < gridH; r++)
	{
		for (int c = 0; c < gridW; c++)
		{
			if (board[r][c].cellType == CellType::BLOCKED) { continue; }
			if (StateCount(Vector2D(c, r), State::IMPOSSIBLE, State::IMPOSSIBLE) > 2)
			{
				blocked = SetType(Vector2D(c, r), CellType::BLOCKED);
				applicable = applicable || blocked;
			}
		}
	}
	return applicable;
}

bool cMain::BParallelConnections()
{
	bool applicable = false;

	for (int c = 0; c < gridW; c++)
	{
		// Count rails in col
		int railNum = 0;
		for (int r = 0; r < gridH; r++)
		{
			if (ReadType(Vector2D(c, r)) == CellType::RAIL)
			{
				railNum++;
			}
		}

		// Col is one off
		if (railNum == colLabels[c] - 1)
		{
			// Loop through rails in col (c, r)
			for (int r = 0; r < gridH; r++)
			{
				if (ReadType(Vector2D(c, r)) != CellType::RAIL) { continue; }

				// Count connections
				if (StateCount(Vector2D(c, r), State::POSSIBLE, State::POSSIBLE) != 2) { continue; }

				// Check connections are parallel
				if (ReadTrack(Vector2D(c, r), "up") == State::POSSIBLE && ReadTrack(Vector2D(c, r), "down") == State::POSSIBLE) {}
				else { continue; }

				// Check neighboring cells are empty
				bool works = true;
				works = works && (ReadType(Vector2D(c, r) + DirVal("up")) == CellType::UNKNOWN);
				works = works && (ReadType(Vector2D(c, r) - DirVal("up")) == CellType::UNKNOWN);

				// Block col
				if (works)
				{
					for (int rBlock = 0; rBlock < gridH; rBlock++)
					{
						if (abs(rBlock - r) > 1 && ReadType(Vector2D(c, rBlock)) == CellType::UNKNOWN)
						{
							bool blocked = SetType(Vector2D(c, rBlock), CellType::BLOCKED);
							applicable = applicable || blocked;
						}
					}
				}
			}
		}
	}
	for (int r = 0; r < gridH; r++)
	{
		// Count rails in row
		int railNum = 0;
		for (int c = 0; c < gridW; c++)
		{
			if (ReadType(Vector2D(c, r)) == CellType::RAIL)
			{
				railNum++;
			}
		}

		// Row is one off
		if (railNum == rowLabels[r] - 1)
		{
			// Loop through rails in row (c, r)
			for (int c = 0; c < gridW; c++)
			{
				if (ReadType(Vector2D(c, r)) != CellType::RAIL) { continue; }

				// Count connections
				if (StateCount(Vector2D(c, r), State::POSSIBLE, State::POSSIBLE) != 2) { continue; }

				// Check connections are parallel
				if (ReadTrack(Vector2D(c, r), "right") == State::POSSIBLE && ReadTrack(Vector2D(c, r), "left") == State::POSSIBLE) {}
				else { continue; }

				// Check neighboring cells are empty
				bool works = true;
				works = works && (ReadType(Vector2D(c, r) + DirVal("right")) == CellType::UNKNOWN);
				works = works && (ReadType(Vector2D(c, r) - DirVal("right")) == CellType::UNKNOWN);

				// Block row
				if (works)
				{
					for (int cBlock = 0; cBlock < gridW; cBlock++)
					{
						if (abs(cBlock - c) > 1 && ReadType(Vector2D(cBlock, r)) == CellType::UNKNOWN)
						{
							bool blocked = SetType(Vector2D(cBlock, r), CellType::BLOCKED);
							applicable = applicable || blocked;
						}
					}
				}
			}
		}
	}
	return applicable;
}

bool cMain::BImpossibleSide()
{
	int railNum;
	bool applicable = false;
	bool blocked;
	for (int c = 0; c < gridW; c++)
	{
		railNum = 0;
		for (int r = 0; r < gridH; r++)
		{
			if (ReadType(Vector2D(c, r)) == CellType::RAIL)
			{
				railNum++;
			}
		}
		if (railNum == colLabels[c] - 1)
		{
			for (int r = 0; r < gridH; r++)
			{
				if (ReadType(Vector2D(c, r)) == CellType::RAIL) { continue; }
				bool sideBlocked = false;
				if (ReadTrack(Vector2D(c + 1, r), "left") == State::IMPOSSIBLE) { sideBlocked = true; }
				if (ReadTrack(Vector2D(c - 1, r), "right") == State::IMPOSSIBLE) { sideBlocked = true; }


				if (sideBlocked)
				{
					bool nextToRail = false;
					if (ReadType(Vector2D(c, r - 1)) == CellType::RAIL && ReadTrack(Vector2D(c, r - 1), "down") != State::IMPOSSIBLE) { nextToRail = true; }
					if (ReadType(Vector2D(c, r + 1)) == CellType::RAIL && ReadTrack(Vector2D(c, r + 1), "up") != State::IMPOSSIBLE) { nextToRail = true; }

					if (!nextToRail)
					{
						blocked = SetType(Vector2D(c, r), CellType::BLOCKED);
						applicable = applicable || blocked;
					}
				}
			}
		}
	}
	for (int r = 0; r < gridH; r++)
	{
		railNum = 0;
		for (int c = 0; c < gridW; c++)
		{
			if (ReadType(Vector2D(c, r)) == CellType::RAIL)
			{
				railNum++;
			}
		}
		if (railNum == rowLabels[r] - 1)
		{
			for (int c = 0; c < gridW; c++)
			{
				if (ReadType(Vector2D(c, r)) == CellType::RAIL) { continue; }
				bool sideBlocked = false;
				if (ReadTrack(Vector2D(c, r - 1), "down") == State::IMPOSSIBLE) { sideBlocked = true; }
				if (ReadTrack(Vector2D(c, r + 1), "up") == State::IMPOSSIBLE) { sideBlocked = true; }


				if (sideBlocked)
				{
					bool nextToRail = false;
					if (ReadType(Vector2D(c - 1, r)) == CellType::RAIL && ReadTrack(Vector2D(c - 1, r), "right") != State::IMPOSSIBLE) { nextToRail = true; }
					if (ReadType(Vector2D(c + 1, r)) == CellType::RAIL && ReadTrack(Vector2D(c + 1, r), "left") != State::IMPOSSIBLE) { nextToRail = true; }

					if (!nextToRail)
					{
						blocked = SetType(Vector2D(c, r), CellType::BLOCKED);
						applicable = applicable || blocked;
					}
				}
			}
		}
	}
	return applicable;
}

bool cMain::BDoubleImpossibleSide()
{
	int railNum;
	bool applicable = false;
	bool blocked;
	for (int c = 0; c < gridW; c++)
	{
		railNum = 0;
		for (int r = 0; r < gridH; r++)
		{
			if (ReadType(Vector2D(c, r)) == CellType::RAIL)
			{
				railNum++;
			}
		}
		if (railNum >= colLabels[c] - 2)
		{
			for (int r = 0; r < gridH; r++)
			{
				if (ReadType(Vector2D(c, r)) == CellType::RAIL) { continue; }
				bool sideBlocked = ReadTrack(Vector2D(c + 1, r), "left") == State::IMPOSSIBLE
					&& ReadTrack(Vector2D(c - 1, r), "right") == State::IMPOSSIBLE;


				if (sideBlocked)
				{
					int nextToRail = 0;
					if (ReadType(Vector2D(c, r - 1)) == CellType::RAIL && ReadTrack(Vector2D(c, r - 1), "down") != State::IMPOSSIBLE) { nextToRail++; }
					if (ReadType(Vector2D(c, r + 1)) == CellType::RAIL && ReadTrack(Vector2D(c, r + 1), "up") != State::IMPOSSIBLE) { nextToRail++; }

					if ((railNum == colLabels[c] - 2 && nextToRail == 0) || (railNum == colLabels[c] - 1 && nextToRail <= 1))
					{
						blocked = SetType(Vector2D(c, r), CellType::BLOCKED);
						applicable = applicable || blocked;
					}
				}
			}
		}
	}
	for (int r = 0; r < gridH; r++)
	{
		railNum = 0;
		for (int c = 0; c < gridW; c++)
		{
			if (ReadType(Vector2D(c, r)) == CellType::RAIL)
			{
				railNum++;
			}
		}
		if (railNum >= rowLabels[r] - 2)
		{
			for (int c = 0; c < gridW; c++)
			{
				if (ReadType(Vector2D(c, r)) == CellType::RAIL) { continue; }
				bool sideBlocked = ReadTrack(Vector2D(c, r - 1), "down") == State::IMPOSSIBLE
					&& ReadTrack(Vector2D(c, r + 1), "up") == State::IMPOSSIBLE;

				if (sideBlocked)
				{
					int nextToRail = 0;
					if (ReadType(Vector2D(c - 1, r)) == CellType::RAIL && ReadTrack(Vector2D(c - 1, r), "right") != State::IMPOSSIBLE) { nextToRail++; }
					if (ReadType(Vector2D(c + 1, r)) == CellType::RAIL && ReadTrack(Vector2D(c + 1, r), "left") != State::IMPOSSIBLE) { nextToRail++; }

					if ((railNum == rowLabels[r] - 2 && nextToRail == 0) || (railNum == rowLabels[r] - 1 && nextToRail <= 1))
					{
						blocked = SetType(Vector2D(c, r), CellType::BLOCKED);
						applicable = applicable || blocked;
					}
				}
			}
		}
	}
	return applicable;
}

bool cMain::BClosedLoop()
{
	bool applicable = false;
	bool trackSet;
	int pathLength = 1;
	Vector2D startPos, currentPos;

	for (int r = 0; r < gridH; r++)
	{
		for (int c = 0; c < gridW; c++)
		{
			startPos = Vector2D(c, r);
			currentPos = startPos;
			if (ReadType(startPos) != CellType::RAIL) { continue; }
			if (StateCount(startPos, State::CERTAIN, State::GIVEN) != 1) { continue; }

			bool pathContinues = true;
			std::string backwardsDir = "";
			std::string tempBackwardsDir = "";
			pathLength = 1;
			while (pathContinues)
			{
				//cout << currentPos.x << "," << currentPos.y << std::endl;
				pathContinues = false;

				std::unordered_map<std::string, State>::iterator it;
				Cell& cellRef = board[currentPos.y][currentPos.x];
				for (it = cellRef.tracks.begin(); it != cellRef.tracks.end(); it++)
				{
					if (it->first == backwardsDir) { continue; }
					if (it->second == State::CERTAIN || it->second == State::GIVEN)
					{
						pathContinues = true;
						pathLength++;
						currentPos += DirVal(it->first);
						tempBackwardsDir = GetOppositeDir(it->first);
					}
				}
				backwardsDir = tempBackwardsDir;

				if (pathLength == 2) { continue; }
				std::string relDir;
				if (NextTo(relDir, startPos, currentPos))
				{
					trackSet = SetTrack(startPos, relDir, State::IMPOSSIBLE);
					applicable = applicable || trackSet;
				}
				if (pathLength > gridW * gridH) { break; }
				if (!InsideGrid(currentPos)) { break; }
			}
		}
	}
	return applicable;
}

bool cMain::IsComplete()
{
	int totalRails = 0;
	bool labelsCorrect = true;
	bool foundFirst = false;
	bool complete = false;

	Vector2D pos1 = Vector2D(-1, -1);
	Vector2D pos2 = Vector2D(-1, -1);

	std::string backwardsDir = "";
	std::string tempBackwardsDir = "";

	for (int c = 0; c < gridW; c++)
	{
		totalRails += colLabels[c];

		int colNum = 0;
		for (int r = 0; r < gridW; r++)
		{
			if (ReadType(Vector2D(c, r)) == CellType::RAIL) { colNum++; }
		}
		labelsCorrect = labelsCorrect && (colNum == colLabels[c]);
	}

	for (int r = 0; r < gridH; r++)
	{
		int rowNum = 0;
		for (int c = 0; c < gridW; c++)
		{
			if (ReadType(Vector2D(c, r)) == CellType::RAIL) { rowNum++; }
		}
		labelsCorrect = labelsCorrect && (rowNum == rowLabels[r]);
	}

	for (int c = 0; c < gridW; c++)
	{
		if (ReadTrack(Vector2D(c, 0), "up") == State::GIVEN)
		{
			backwardsDir = "up";
			if (!foundFirst)
			{
				pos1 = Vector2D(c, 0);
				foundFirst = true;
			}
			else
			{
				pos2 = Vector2D(c, 0);
			}
		}
		if (ReadTrack(Vector2D(c, gridH - 1), "down") == State::GIVEN)
		{
			backwardsDir = "down";
			if (!foundFirst)
			{
				pos1 = Vector2D(c, gridH - 1);
				foundFirst = true;
			}
			else
			{
				pos2 = Vector2D(c, gridH - 1);
			}
		}
	}

	for (int r = 0; r < gridH; r++)
	{
		if (ReadTrack(Vector2D(0, r), "left") == State::GIVEN)
		{
			backwardsDir = "left";
			if (!foundFirst)
			{
				pos1 = Vector2D(0, r);
				foundFirst = true;
			}
			else
			{
				pos2 = Vector2D(0, r);
			}
		}
		if (ReadTrack(Vector2D(gridW - 1, r), "right") == State::GIVEN)
		{
			backwardsDir = "right";
			if (!foundFirst)
			{
				pos1 = Vector2D(gridW - 1, r);
				foundFirst = true;
			}
			else
			{
				pos2 = Vector2D(gridW - 1, r);
			}
		}
	}

	Vector2D currentPos = pos1;
	bool pathContinues = true;
	int railsPassed = 1;
	while (pathContinues)
	{
		pathContinues = false;

		std::unordered_map<std::string, State>::iterator it;
		if (!InsideGrid(currentPos)) { return false; }
		Cell& cellRef = board[currentPos.y][currentPos.x];
		for (it = cellRef.tracks.begin(); it != cellRef.tracks.end(); it++)
		{
			if (it->first == backwardsDir) { continue; }
			if (it->second == State::CERTAIN || it->second == State::GIVEN)
			{
				pathContinues = true;
				currentPos += DirVal(it->first);
				railsPassed++;
				tempBackwardsDir = GetOppositeDir(it->first);
			}
		}
		backwardsDir = tempBackwardsDir;

		if (currentPos == pos2)
		{
			complete = true;
			break;
		}
		if (railsPassed > totalRails)
		{
			complete = false;
			break;
		}
	}

	return labelsCorrect && complete && (railsPassed == totalRails);
}

int recursionDepth = 0;
int cMain::SolutionNum(std::vector<std::vector<Cell>>& boardIn)
{
	recursionDepth++;
	int solutionNum = 0;
	board = boardIn;
	unsolvable = false;
	Solve();
	std::vector<std::vector<Cell>> boardBackup = board;

	//cout << "Unsolvable: " << unsolvable << std::endl;
	bool isComplete = IsComplete();
	cout << "Depth " << recursionDepth << ": " << isComplete << std::endl;
	if (isComplete) { recursionDepth--; return 1; }
	if (unsolvable) { recursionDepth--; return 0; }

	bool foundBranch = false;
	Vector2D branchPos = Vector2D();

	int uncertainRails = 0;
	for (int c = 0; c < gridW; c++)
	{
		for (int r = 0; r < gridH; r++)
		{
			if (ReadType(Vector2D(c, r)) == CellType::RAIL)
			{
				uncertainRails += StateCount(Vector2D(c, r), State::POSSIBLE, State::POSSIBLE);
				if (StateCount(Vector2D(c, r), State::POSSIBLE, State::POSSIBLE) >= 2 && !foundBranch)
				{
					branchPos = Vector2D(c, r);
					foundBranch = true;
				}
			}
		}
	}
	if (uncertainRails == 0) { recursionDepth--; return 0; }

	if (recursionDepth == 1) { cout << "Branch " << branchPos.x << ", " << branchPos.y << std::endl; }

	std::vector<std::string> dirs{"up", "down", "right", "left"};
	for (std::string dir : dirs)
	{
		cout << "Track in " << dir << ": " << (int)board[branchPos.y][branchPos.x].tracks[dir] << std::endl;
		if (ReadTrack(branchPos, dir) == State::POSSIBLE)
		{
			SetTrack(branchPos, dir, State::CERTAIN);

			solutionNum += SolutionNum(board);

			board = boardBackup;
		}
		cout << "Track out " << dir << ": " << (int)board[branchPos.y][branchPos.x].tracks[dir] << std::endl;
	}

	recursionDepth--;
	return solutionNum;
}