#include "Main.h"

Main::Main()
{
	gridW = 6;
	gridH = 6;

	NewBoard();
	NewLabels();

	unsolvable = false;
	display = true;
}

void Main::NewBoard()
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

void Main::NewLabels()
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

bool Main::Iterate()
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

void Main::Solve()
{
	bool done = false;

	while (!done && !unsolvable)
	{
		done = Iterate();
		done = !done;
	}
}

Direction Main::GetOppositeDir(Direction dir)
{
	static constexpr Direction opposites[] = { Direction::DOWN, Direction::UP, Direction::LEFT, Direction::RIGHT };
	return opposites[(int)dir];
}

std::optional<Direction> Main::NextTo(Vector2D first, Vector2D second)
{
	Vector2D del = second - first;
	bool nextTo = (std::abs(del.x) + std::abs(del.y) == 1);

	if (nextTo)
		return VecToDir(del);
	else
		return {};
}

Vector2D Main::DirToVec(Direction dir)
{
	switch (dir)
	{
	case Direction::UP:
		return { 0, -1 };
		break;
	case Direction::DOWN:
		return { 0, 1 };
		break;
	case Direction::RIGHT:
		return { 1, 0 };
		break;
	case Direction::LEFT:
		return { -1, 0 };
		break;
	}
}

Direction Main::VecToDir(Vector2D dir)
{
	assert(dir.IsUnit());

	switch (dir.x)
	{
	case 0:
		switch (dir.y)
		{
		case -1:
			return Direction::UP;
		case 1:
			return Direction::DOWN;
		}
	case -1:
		return Direction::LEFT;
	case 1:
		return Direction::RIGHT;
	}
}

bool Main::SetType(Vector2D pos, CellType newType)
{
	if (!InsideGrid(pos)) return false;

	Cell& cell = board[pos.y][pos.x];
	bool applicable = (cell.cellType != newType);

	// Check if assignment violates logic
	if (cell.cellType == CellType::BLOCKED && newType == CellType::RAIL)
	{
		unsolvable = true;
		return false;
	}
	if (cell.cellType == CellType::RAIL && newType == CellType::BLOCKED)
	{
		unsolvable = true;
		return false;
	}

	// Assign new type
	cell.cellType = newType;

	// if blocked, set all rails within to 'IMPOSSIBLE'
	if (newType == CellType::BLOCKED)
	{
		for (int i = 0; i < 4; i++)
			cell.tracks[i] = State::IMPOSSIBLE;
	}

	return applicable;
}

bool Main::SetTrack(Vector2D pos, Direction dir, State newState)
{
	if (!InsideGrid(pos)) return false;

	bool applicable = false;
	State& currentState = board[pos.y][pos.x].tracks[(int)dir];

	if (currentState == State::GIVEN) { return false; }
	if (currentState == State::CERTAIN && newState == State::IMPOSSIBLE)
	{
		unsolvable = true;
		return false;
	}
	if (currentState == State::IMPOSSIBLE && newState == State::CERTAIN)
	{
		unsolvable = true;
		return false;
	}

	applicable = (currentState != newState);
	currentState = newState;

	if (board[pos.y][pos.x].cellType == CellType::UNKNOWN) { applicable = false; }

	return applicable;
}

CellType Main::ReadType(Vector2D pos)
{
	if (!InsideGrid(pos)) return CellType::BLOCKED;
	return board[pos.y][pos.x].cellType;
}

CellType Main::ReadType(int x, int y)
{
	return ReadType({x, y});
}

State Main::ReadTrack(Vector2D pos, Direction dir)
{
	if (!InsideGrid(pos)) return State::IMPOSSIBLE;
	return board[pos.y][pos.x].tracks[(int)dir];
}

State Main::ReadTrack(int x, int y, Direction dir)
{
	return ReadTrack({x, y}, dir);
}

int Main::StateCount(Vector2D pos, State s1, State s2)
{
	int count = 0;
	for (int i = 0; i < 4; i++)
	{
		State state = board[pos.y][pos.x].tracks[i];
		if (state == s1 || state == s2)
			count++;
	}
	return count;
}

int Main::StateCount(int x, int y, State s1, State s2)
{
	return StateCount({x, y}, s1, s2);
}

bool Main::BBlockedTrack()
{
	bool applicable = false;
	for (int r = 0; r < gridH; r++)
	{
		for (int c = 0; c < gridW; c++)
		{
			for (int i = 0; i < 4; i++)
			{
				Vector2D nextPos = Vector2D(c, r);
				nextPos += DirToVec((Direction)i);
				if (ReadTrack(nextPos, GetOppositeDir((Direction)i)) == State::IMPOSSIBLE)
					applicable = SetTrack(Vector2D(c, r), (Direction)i, State::IMPOSSIBLE);
			}
		}
	}
	return applicable;
}

bool Main::PRailHead()
{
	bool applicable = false;
	bool setType, setTrack;
	for (int r = 0; r < gridH; r++)
	{
		for (int c = 0; c < gridW; c++)
		{
			CellType cellType = ReadType(c, r);
			if (cellType == CellType::UNKNOWN || cellType == CellType::BLOCKED) continue;

			for (int i = 0; i < 4; i++)
			{
				State state = board[r][c].tracks[i];
				if (state == State::CERTAIN || state == State::GIVEN)
				{
					Vector2D nextPos = Vector2D(c, r);
					nextPos += DirToVec((Direction)i);
					setType = SetType(nextPos, CellType::RAIL);
					setTrack = SetTrack(nextPos, GetOppositeDir((Direction)i), State::CERTAIN);
					applicable = applicable || setType || setTrack;
				}
			}
		}
	}
	return applicable;
}

bool Main::BFullRow()
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
		if (railNum > colLabels[c]) { unsolvable = true; /*throw; std::cout << "Col: " << c;*/ }
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
		if (railNum > rowLabels[r]) { unsolvable = true; /*throw; std::cout << "Row: " << r;*/ }
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

bool Main::PFixedDir()
{
	bool applicable = false;
	bool setTrack;
	for (int r = 0; r < gridH; r++)
	{
		for (int c = 0; c < gridW; c++)
		{
			if (ReadType(c, r) != CellType::RAIL) { continue; }
			if (StateCount(c, r, State::IMPOSSIBLE, State::IMPOSSIBLE) == 2)
			{
				for (int i = 0; i < 4; i++)
				{
					if (board[r][c].tracks[i] == State::POSSIBLE)
					{
						setTrack = SetTrack(Vector2D(c, r), (Direction)i, State::CERTAIN);
						applicable = applicable || setTrack;
					}
				}
			}
			if (StateCount(c, r, State::CERTAIN, State::CERTAIN) == 2)
			{
				for (int i = 0; i < 4; i++)
				{
					if (board[r][c].tracks[i] == State::POSSIBLE)
					{
						setTrack = SetTrack(Vector2D(c, r), (Direction)i, State::IMPOSSIBLE);
						applicable = applicable || setTrack;
					}
				}
			}
		}
	}
	return applicable;
}

bool Main::PNSpaces()
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

bool Main::BDeadEnd()
{
	bool applicable = false;
	bool blocked;
	for (int r = 0; r < gridH; r++)
	{
		for (int c = 0; c < gridW; c++)
		{
			if (board[r][c].cellType == CellType::BLOCKED) { continue; }
			if (StateCount(c, r, State::IMPOSSIBLE, State::IMPOSSIBLE) > 2)
			{
				blocked = SetType(Vector2D(c, r), CellType::BLOCKED);
				applicable = applicable || blocked;
			}
		}
	}
	return applicable;
}

bool Main::BParallelConnections()
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
				if (StateCount(c, r, State::POSSIBLE, State::POSSIBLE) != 2) { continue; }

				// Check connections are parallel
				if (ReadTrack(c, r, Direction::UP) != State::POSSIBLE || ReadTrack(c, r, Direction::DOWN) != State::POSSIBLE)
					continue;

				// Check neighboring cells are empty
				bool works = true;
				works &= (ReadType(Vector2D(c, r) + DirToVec(Direction::UP)) == CellType::UNKNOWN);
				works &= (ReadType(Vector2D(c, r) - DirToVec(Direction::UP)) == CellType::UNKNOWN);

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
				if (StateCount(c, r, State::POSSIBLE, State::POSSIBLE) != 2) { continue; }

				// Check connections are parallel
				if (ReadTrack(c, r, Direction::RIGHT) == State::POSSIBLE && ReadTrack(c, r, Direction::LEFT) == State::POSSIBLE) {}
				else { continue; }

				// Check neighboring cells are empty
				bool works = true;
				works = works && (ReadType(Vector2D(c, r) + DirToVec(Direction::RIGHT)) == CellType::UNKNOWN);
				works = works && (ReadType(Vector2D(c, r) - DirToVec(Direction::RIGHT)) == CellType::UNKNOWN);

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

bool Main::BImpossibleSide()
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
				if (ReadTrack(Vector2D(c + 1, r), Direction::LEFT) == State::IMPOSSIBLE) { sideBlocked = true; }
				if (ReadTrack(Vector2D(c - 1, r), Direction::RIGHT) == State::IMPOSSIBLE) { sideBlocked = true; }


				if (sideBlocked)
				{
					bool nextToRail = false;
					if (ReadType(Vector2D(c, r - 1)) == CellType::RAIL && ReadTrack(Vector2D(c, r - 1), Direction::DOWN) != State::IMPOSSIBLE) { nextToRail = true; }
					if (ReadType(Vector2D(c, r + 1)) == CellType::RAIL && ReadTrack(Vector2D(c, r + 1), Direction::UP) != State::IMPOSSIBLE) { nextToRail = true; }

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
				if (ReadTrack(Vector2D(c, r - 1), Direction::DOWN) == State::IMPOSSIBLE) { sideBlocked = true; }
				if (ReadTrack(Vector2D(c, r + 1), Direction::UP) == State::IMPOSSIBLE) { sideBlocked = true; }


				if (sideBlocked)
				{
					bool nextToRail = false;
					if (ReadType(Vector2D(c - 1, r)) == CellType::RAIL && ReadTrack(Vector2D(c - 1, r), Direction::RIGHT) != State::IMPOSSIBLE) { nextToRail = true; }
					if (ReadType(Vector2D(c + 1, r)) == CellType::RAIL && ReadTrack(Vector2D(c + 1, r), Direction::LEFT) != State::IMPOSSIBLE) { nextToRail = true; }

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

bool Main::BDoubleImpossibleSide()
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
				bool sideBlocked = ReadTrack(Vector2D(c + 1, r), Direction::LEFT) == State::IMPOSSIBLE
					&& ReadTrack(Vector2D(c - 1, r), Direction::RIGHT) == State::IMPOSSIBLE;


				if (sideBlocked)
				{
					int nextToRail = 0;
					if (ReadType(Vector2D(c, r - 1)) == CellType::RAIL && ReadTrack(Vector2D(c, r - 1), Direction::DOWN) != State::IMPOSSIBLE) { nextToRail++; }
					if (ReadType(Vector2D(c, r + 1)) == CellType::RAIL && ReadTrack(Vector2D(c, r + 1), Direction::UP) != State::IMPOSSIBLE) { nextToRail++; }

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
				bool sideBlocked = ReadTrack(Vector2D(c, r - 1), Direction::DOWN) == State::IMPOSSIBLE
					&& ReadTrack(Vector2D(c, r + 1), Direction::UP) == State::IMPOSSIBLE;

				if (sideBlocked)
				{
					int nextToRail = 0;
					if (ReadType(Vector2D(c - 1, r)) == CellType::RAIL && ReadTrack(Vector2D(c - 1, r), Direction::RIGHT) != State::IMPOSSIBLE) { nextToRail++; }
					if (ReadType(Vector2D(c + 1, r)) == CellType::RAIL && ReadTrack(Vector2D(c + 1, r), Direction::LEFT) != State::IMPOSSIBLE) { nextToRail++; }

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

bool Main::BClosedLoop()
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
			Direction backwardsDir = Direction::NONE;
			Direction tempBackwardsDir = Direction::NONE;
			pathLength = 1;
			while (pathContinues)
			{
				pathContinues = false;

				Cell& cellRef = board[currentPos.y][currentPos.x];
				for (int i = 0; i < 4; i++)
				{
					if ((Direction)i == backwardsDir) { continue; }
					State state = cellRef.tracks[i];
					if (state == State::CERTAIN || state == State::GIVEN)
					{
						pathContinues = true;
						pathLength++;
						currentPos += DirToVec((Direction)i);
						tempBackwardsDir = GetOppositeDir((Direction)i);
					}
				}
				backwardsDir = tempBackwardsDir;

				if (pathLength == 2) { continue; }
				auto relDir = NextTo(startPos, currentPos);
				if (relDir.has_value())
				{
					trackSet = SetTrack(startPos, relDir.value(), State::IMPOSSIBLE);
					applicable = applicable || trackSet;
				}
				if (pathLength > gridW * gridH) { break; }
				if (!InsideGrid(currentPos)) { break; }
			}
		}
	}
	return applicable;
}

bool Main::IsComplete()
{
	int totalRails = 0;
	bool labelsCorrect = true;
	bool foundFirst = false;
	bool complete = false;

	Vector2D pos1 = Vector2D(-1, -1);
	Vector2D pos2 = Vector2D(-1, -1);

	Direction backwardsDir = Direction::NONE;
	Direction tempBackwardsDir = Direction::NONE;

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
		if (ReadTrack(Vector2D(c, 0), Direction::UP) == State::GIVEN)
		{
			backwardsDir = Direction::UP;
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
		if (ReadTrack(Vector2D(c, gridH - 1), Direction::DOWN) == State::GIVEN)
		{
			backwardsDir = Direction::DOWN;
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
		if (ReadTrack(Vector2D(0, r), Direction::LEFT) == State::GIVEN)
		{
			backwardsDir = Direction::LEFT;
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
		if (ReadTrack(Vector2D(gridW - 1, r), Direction::RIGHT) == State::GIVEN)
		{
			backwardsDir = Direction::RIGHT;
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
		for (int i = 0; i < 4; i++)
		{
			if ((Direction)i == backwardsDir) { continue; }

			State state = cellRef.tracks[i];
			if (state == State::CERTAIN || state == State::GIVEN)
			{
				pathContinues = true;
				currentPos += DirToVec((Direction)i);
				railsPassed++;
				tempBackwardsDir = GetOppositeDir((Direction)i);
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
int Main::SolutionNum(std::vector<std::vector<Cell>>& boardIn)
{
	recursionDepth++;
	int solutionNum = 0;
	board = boardIn;
	unsolvable = false;
	Solve();
	std::vector<std::vector<Cell>> boardBackup = board;

	//std::cout << "Unsolvable: " << unsolvable << std::endl;
	bool isComplete = IsComplete();
	std::cout << "Depth " << recursionDepth << ": " << isComplete << std::endl;
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
				uncertainRails += StateCount(c, r, State::POSSIBLE, State::POSSIBLE);
				if (StateCount(c, r, State::POSSIBLE, State::POSSIBLE) >= 2 && !foundBranch)
				{
					branchPos = Vector2D(c, r);
					foundBranch = true;
				}
			}
		}
	}
	if (uncertainRails == 0) { recursionDepth--; return 0; }

	if (recursionDepth == 1) { std::cout << "Branch " << branchPos.x << ", " << branchPos.y << std::endl; }

	Direction dirs[] = {Direction::UP, Direction::DOWN, Direction::RIGHT, Direction::LEFT};
	for (Direction dir : dirs)
	{
		std::cout << "Track in " << (int)dir << ": " << (int)board[branchPos.y][branchPos.x].tracks[(int)dir] << std::endl;
		if (ReadTrack(branchPos, dir) == State::POSSIBLE)
		{
			SetTrack(branchPos, dir, State::CERTAIN);

			solutionNum += SolutionNum(board);

			board = boardBackup;
		}
		std::cout << "Track out " << (int)dir << ": " << (int)board[branchPos.y][branchPos.x].tracks[(int)dir] << std::endl;
	}

	recursionDepth--;
	return solutionNum;
}