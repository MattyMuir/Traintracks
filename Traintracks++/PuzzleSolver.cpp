#include "PuzzleSolver.h"

PuzzleSolver::PuzzleSolver(Board* boardPtr_)
	: boardPtr(boardPtr_) {}

StrategyResult PuzzleSolver::TakeStep()
{
	StrategyResult last = StrategyResult::UNSUITABLE;

	if (last == StrategyResult::UNSUITABLE) { last = PRailHead(); }
	if (last == StrategyResult::UNSUITABLE) { last = BBlockedTrack(); }
	if (last == StrategyResult::UNSUITABLE) { last = BFullRow(); }
	if (last == StrategyResult::UNSUITABLE) { last = PFixedDir(); }
	if (last == StrategyResult::UNSUITABLE) { last = PNSpaces(); }
	if (last == StrategyResult::UNSUITABLE) { last = BDeadEnd(); }
	if (last == StrategyResult::UNSUITABLE) { last = BParallelConnections(); }
	if (last == StrategyResult::UNSUITABLE) { last = BImpossibleSide(); }
	if (last == StrategyResult::UNSUITABLE) { last = BDoubleImpossibleSide(); }
	if (last == StrategyResult::UNSUITABLE) { last = BClosedLoop(); }

	return last;
}

StrategyResult PuzzleSolver::Solve()
{
	TIMER(solve);

	StrategyResult last;
	do
	{
		last = TakeStep();
	} while (last == StrategyResult::SUCCESS);

	if (last == StrategyResult::FOUND_FLAW) return StrategyResult::FOUND_FLAW;

	bool isSolved = IsSolved();

	STOP_LOG(solve);

	if (isSolved)
		return StrategyResult::SUCCESS;
	else
		return StrategyResult::UNSUITABLE;
}

bool PuzzleSolver::IsSolved() const
{
	const Board& board = *boardPtr;

	int totalRails = 0;
	bool labelsCorrect = true;
	bool foundFirst = false;
	bool complete = false;

	IntVec pos1 = { -1, -1 };
	IntVec pos2 = { -1, -1 };

	Direction backwardsDir = Direction::NONE;
	Direction tempBackwardsDir = Direction::NONE;

	for (int c = 0; c < board.w; c++)
	{
		totalRails += board.colLabels[c];

		int colNum = 0;
		for (int r = 0; r < board.w; r++)
		{
			if (ReadType(c, r) == CellType::RAIL) { colNum++; }
		}
		labelsCorrect = labelsCorrect && (colNum == board.colLabels[c]);
	}

	for (int r = 0; r < board.h; r++)
	{
		int rowNum = 0;
		for (int c = 0; c < board.w; c++)
		{
			if (ReadType(c, r) == CellType::RAIL) { rowNum++; }
		}
		labelsCorrect = labelsCorrect && (rowNum == board.rowLabels[r]);
	}

	for (int c = 0; c < board.w; c++)
	{
		if (ReadTrack(c, 0, Direction::UP) == State::GIVEN)
		{
			backwardsDir = Direction::UP;
			if (!foundFirst)
			{
				pos1 = { c, 0 };
				foundFirst = true;
			}
			else
			{
				pos2 = { c, 0 };
			}
		}
		if (ReadTrack(c, board.h - 1, Direction::DOWN) == State::GIVEN)
		{
			backwardsDir = Direction::DOWN;
			if (!foundFirst)
			{
				pos1 = { c, board.h - 1 };
				foundFirst = true;
			}
			else
			{
				pos2 = { c, board.h - 1 };
			}
		}
	}

	for (int r = 0; r < board.h; r++)
	{
		if (ReadTrack(0, r, Direction::LEFT) == State::GIVEN)
		{
			backwardsDir = Direction::LEFT;
			if (!foundFirst)
			{
				pos1 = { 0, r };
				foundFirst = true;
			}
			else
			{
				pos2 = { 0, r };
			}
		}
		if (ReadTrack(board.w - 1, r, Direction::RIGHT) == State::GIVEN)
		{
			backwardsDir = Direction::RIGHT;
			if (!foundFirst)
			{
				pos1 = { board.w - 1, r };
				foundFirst = true;
			}
			else
			{
				pos2 = { board.w - 1, r };
			}
		}
	}

	IntVec currentPos = pos1;
	bool pathContinues = true;
	int railsPassed = 1;
	while (pathContinues)
	{
		pathContinues = false;

		if (!board.In(currentPos)) { return false; }
		const Cell& cellRef = board[currentPos.y][currentPos.x];
		for (int i = 0; i < 4; i++)
		{
			if ((Direction)i == backwardsDir) { continue; }

			State state = cellRef.tracks[i];
			if (state == State::CERTAIN || state == State::GIVEN)
			{
				pathContinues = true;
				currentPos += DirToVec((Direction)i);
				railsPassed++;
				tempBackwardsDir = Opposite((Direction)i);
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

Direction PuzzleSolver::Opposite(Direction dir)
{
	static constexpr Direction opposites[] = { Direction::DOWN, Direction::UP, Direction::LEFT, Direction::RIGHT };
	return opposites[(int)dir];
}

std::optional<Direction> PuzzleSolver::NextTo(IntVec first, IntVec second)
{
	IntVec del = second - first;
	bool nextTo = del.IsUnit();

	if (nextTo)
		return VecToDir(del);
	else
		return {};
}

IntVec PuzzleSolver::DirToVec(Direction dir)
{
	assert(dir != Direction::NONE);

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

	return { 0, 0 };
}

Direction PuzzleSolver::VecToDir(IntVec dir)
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

	return Direction::NONE;
}

StrategyResult PuzzleSolver::SetType(int x, int y, CellType newType)
{
	Board& board = *boardPtr;

	if (!board.In(x, y)) return StrategyResult::UNSUITABLE;

	Cell& cell = board[y][x];
	bool changed = (cell.cellType != newType);

	// Check if assignment violates logic
	if (cell.cellType == CellType::BLOCKED && newType == CellType::RAIL)
		return StrategyResult::FOUND_FLAW;
	if (cell.cellType == CellType::RAIL && newType == CellType::BLOCKED)
		return StrategyResult::FOUND_FLAW;

	// Assign new type
	cell.cellType = newType;

	// if blocked, set all rails within to 'IMPOSSIBLE'
	if (newType == CellType::BLOCKED)
	{
		for (int i = 0; i < 4; i++)
			cell.tracks[i] = State::IMPOSSIBLE;
	}

	if (changed)
		return StrategyResult::SUCCESS;
	else
		return StrategyResult::UNSUITABLE;
}

StrategyResult PuzzleSolver::SetType(IntVec pos, CellType newType)
{
	return SetType(pos.x, pos.y, newType);
}

CellType PuzzleSolver::ReadType(int x, int y) const
{
	Board& board = *boardPtr;
	if (!board.In(x, y)) return CellType::BLOCKED;
	return board[y][x].cellType;
}

CellType PuzzleSolver::ReadType(IntVec pos) const
{
	return ReadType(pos.x, pos.y);
}

StrategyResult PuzzleSolver::SetTrack(int x, int y, Direction dir, State newState)
{
	Board& board = *boardPtr;
	if (!board.In(x, y)) return StrategyResult::UNSUITABLE;

	State& currentState = board[y][x].tracks[(int)dir];
	bool changed = (currentState != newState);

	if (currentState == State::GIVEN) { return StrategyResult::UNSUITABLE; }
	if (currentState == State::CERTAIN && newState == State::IMPOSSIBLE)
		return StrategyResult::FOUND_FLAW;
	if (currentState == State::IMPOSSIBLE && newState == State::CERTAIN)
		return StrategyResult::FOUND_FLAW;

	currentState = newState;

	if (board[y][x].cellType == CellType::UNKNOWN) { changed = false; }

	if (changed)
		return StrategyResult::SUCCESS;
	else
		return StrategyResult::UNSUITABLE;
}

StrategyResult PuzzleSolver::SetTrack(IntVec pos, Direction dir, State newState)
{
	return SetTrack(pos.x, pos.y, dir, newState);
}

State PuzzleSolver::ReadTrack(int x, int y, Direction dir) const
{
	Board& board = *boardPtr;
	if (!board.In(x, y)) return State::IMPOSSIBLE;
	return board[y][x].tracks[(int)dir];
}

State PuzzleSolver::ReadTrack(IntVec pos, Direction dir) const
{
	return ReadTrack(pos.x, pos.y, dir);
}

int PuzzleSolver::StateCount(int x, int y, State s1, State s2)
{
	Board& board = *boardPtr;

	int count = 0;
	for (int i = 0; i < 4; i++)
	{
		State state = board[y][x].tracks[i];
		if (state == s1 || state == s2)
			count++;
	}
	return count;
}

int PuzzleSolver::StateCount(IntVec pos, State s1, State s2)
{
	return StateCount(pos.x, pos.y, s1, s2);
}

StrategyResult PuzzleSolver::BBlockedTrack()
{
	StrategyResult applicable = StrategyResult::UNSUITABLE;
	for (int r = 0; r < boardPtr->h; r++)
	{
		for (int c = 0; c < boardPtr->w; c++)
		{
			for (int i = 0; i < 4; i++)
			{
				IntVec nextPos = { c, r };
				nextPos += DirToVec((Direction)i);
				if (ReadTrack(nextPos, Opposite((Direction)i)) == State::IMPOSSIBLE)
				{
					StrategyResult res = SetTrack(c, r, (Direction)i, State::IMPOSSIBLE);
					if (res == StrategyResult::FOUND_FLAW) return StrategyResult::FOUND_FLAW;
					if (res == StrategyResult::SUCCESS) applicable = StrategyResult::SUCCESS;
				}
			}
		}
	}
	return applicable;
}

StrategyResult PuzzleSolver::PRailHead()
{
	Board& board = *boardPtr;

	bool applicable = false;
	StrategyResult setType, setTrack;
	for (int r = 0; r < board.h; r++)
	{
		for (int c = 0; c < board.w; c++)
		{
			CellType cellType = ReadType(c, r);
			if (cellType == CellType::UNKNOWN || cellType == CellType::BLOCKED) continue;

			for (int i = 0; i < 4; i++)
			{
				State state = board[r][c].tracks[i];
				if (state == State::CERTAIN || state == State::GIVEN)
				{
					IntVec nextPos = IntVec(c, r);
					nextPos += DirToVec((Direction)i);
					setType = SetType(nextPos, CellType::RAIL);
					setTrack = SetTrack(nextPos, Opposite((Direction)i), State::CERTAIN);

					if (setType == StrategyResult::FOUND_FLAW) return StrategyResult::FOUND_FLAW;
					if (setTrack == StrategyResult::FOUND_FLAW) return StrategyResult::FOUND_FLAW;

					applicable |= (setType == StrategyResult::SUCCESS) || (setTrack == StrategyResult::SUCCESS);
				}
			}
		}
	}

	if (applicable)
		return StrategyResult::SUCCESS;
	else
		return StrategyResult::UNSUITABLE;
}

StrategyResult PuzzleSolver::BFullRow()
{
	Board& board = *boardPtr;

	bool applicable = false;

	int railNum = 0;
	for (int c = 0; c < board.w; c++)
	{
		railNum = 0;
		for (int r = 0; r < board.h; r++)
			if (ReadType(IntVec(c, r)) == CellType::RAIL) { railNum++; }

		if (railNum > board.colLabels[c]) return StrategyResult::FOUND_FLAW;

		// Block column
		if (railNum == board.colLabels[c])
		{
			for (int r = 0; r < board.h; r++)
			{
				if (ReadType(c, r) == CellType::UNKNOWN)
				{
					StrategyResult res = SetType(IntVec(c, r), CellType::BLOCKED);
					if (res == StrategyResult::FOUND_FLAW) return StrategyResult::FOUND_FLAW;
					applicable |= (res == StrategyResult::SUCCESS);
				}
			}
		}
	}
	for (int r = 0; r < board.h; r++)
	{
		railNum = 0;
		for (int c = 0; c < board.w; c++)
			if (ReadType(IntVec(c, r)) == CellType::RAIL) { railNum++; }

		if (railNum > board.rowLabels[r]) return StrategyResult::FOUND_FLAW;

		// Block row
		if (railNum == board.rowLabels[r])
		{
			for (int c = 0; c < board.w; c++)
			{
				if (ReadType(IntVec(c, r)) == CellType::UNKNOWN)
				{
					StrategyResult res = SetType(IntVec(c, r), CellType::BLOCKED);
					if (res == StrategyResult::FOUND_FLAW) return StrategyResult::FOUND_FLAW;
					applicable |= (res == StrategyResult::SUCCESS);
				}
			}
		}
	}
	
	if (applicable)
		return StrategyResult::SUCCESS;
	else
		return StrategyResult::UNSUITABLE;
}

StrategyResult PuzzleSolver::PFixedDir()
{
	Board& board = *boardPtr;

	bool applicable = false;
	for (int r = 0; r < board.h; r++)
	{
		for (int c = 0; c < board.w; c++)
		{
			if (ReadType(c, r) != CellType::RAIL) { continue; }
			if (StateCount(c, r, State::IMPOSSIBLE, State::IMPOSSIBLE) == 2)
			{
				for (int i = 0; i < 4; i++)
				{
					if (board[r][c].tracks[i] == State::POSSIBLE)
					{
						StrategyResult res = SetTrack(IntVec(c, r), (Direction)i, State::CERTAIN);
						if (res == StrategyResult::FOUND_FLAW) return StrategyResult::FOUND_FLAW;
						applicable |= (res == StrategyResult::SUCCESS);
					}
				}
			}
			if (StateCount(c, r, State::CERTAIN, State::CERTAIN) == 2)
			{
				for (int i = 0; i < 4; i++)
				{
					if (board[r][c].tracks[i] == State::POSSIBLE)
					{
						StrategyResult res = SetTrack(IntVec(c, r), (Direction)i, State::IMPOSSIBLE);
						if (res == StrategyResult::FOUND_FLAW) return StrategyResult::FOUND_FLAW;
						applicable |= (res == StrategyResult::SUCCESS);
					}
				}
			}
		}
	}
	
	if (applicable)
		return StrategyResult::SUCCESS;
	else
		return StrategyResult::UNSUITABLE;
}

StrategyResult PuzzleSolver::PNSpaces()
{
	Board& board = *boardPtr;

	bool applicable = false;

	int blockedNum = 0;
	for (int c = 0; c < board.w; c++)
	{
		blockedNum = 0;
		for (int r = 0; r < board.h; r++)
		{
			if (ReadType(IntVec(c, r)) == CellType::BLOCKED) { blockedNum++; }
		}
		if (blockedNum == board.h - board.colLabels[c])
		{
			for (int r = 0; r < board.h; r++)
			{
				if (ReadType(IntVec(c, r)) == CellType::UNKNOWN)
				{
					StrategyResult res = SetType(IntVec(c, r), CellType::RAIL);
					if (res == StrategyResult::FOUND_FLAW) return StrategyResult::FOUND_FLAW;
					applicable |= (res == StrategyResult::SUCCESS);
				}
			}
		}
	}
	for (int r = 0; r < board.h; r++)
	{
		blockedNum = 0;
		for (int c = 0; c < board.w; c++)
		{
			if (ReadType(IntVec(c, r)) == CellType::BLOCKED) { blockedNum++; }
		}
		if (blockedNum == board.w - board.rowLabels[r])
		{
			for (int c = 0; c < board.w; c++)
			{
				if (ReadType(IntVec(c, r)) == CellType::UNKNOWN)
				{
					StrategyResult res = SetType(IntVec(c, r), CellType::RAIL);
					if (res == StrategyResult::FOUND_FLAW) return StrategyResult::FOUND_FLAW;
					applicable |= (res == StrategyResult::SUCCESS);
				}
			}
		}
	}
	if (applicable)
		return StrategyResult::SUCCESS;
	else
		return StrategyResult::UNSUITABLE;
}

StrategyResult PuzzleSolver::BDeadEnd()
{
	Board& board = *boardPtr;

	bool applicable = false;
	for (int r = 0; r < board.h; r++)
	{
		for (int c = 0; c < board.w; c++)
		{
			if (board[r][c].cellType == CellType::BLOCKED) { continue; }
			if (StateCount(c, r, State::IMPOSSIBLE, State::IMPOSSIBLE) > 2)
			{
				StrategyResult res = SetType(IntVec(c, r), CellType::BLOCKED);
				if (res == StrategyResult::FOUND_FLAW) return StrategyResult::FOUND_FLAW;
				applicable |= (res == StrategyResult::SUCCESS);
			}
		}
	}
	if (applicable)
		return StrategyResult::SUCCESS;
	else
		return StrategyResult::UNSUITABLE;
}

StrategyResult PuzzleSolver::BParallelConnections()
{
	Board& board = *boardPtr;
	bool applicable = false;

	for (int c = 0; c < board.w; c++)
	{
		// Count rails in col
		int railNum = 0;
		for (int r = 0; r < board.h; r++)
		{
			if (ReadType(IntVec(c, r)) == CellType::RAIL)
			{
				railNum++;
			}
		}

		// Col is one off
		if (railNum == board.colLabels[c] - 1)
		{
			// Loop through rails in col (c, r)
			for (int r = 0; r < board.h; r++)
			{
				if (ReadType(IntVec(c, r)) != CellType::RAIL) { continue; }

				// Count connections
				if (StateCount(c, r, State::POSSIBLE, State::POSSIBLE) != 2) { continue; }

				// Check connections are parallel
				if (ReadTrack(c, r, Direction::UP) != State::POSSIBLE || ReadTrack(c, r, Direction::DOWN) != State::POSSIBLE)
					continue;

				// Check neighboring cells are empty
				bool works = true;
				works &= (ReadType(IntVec(c, r) + DirToVec(Direction::UP)) == CellType::UNKNOWN);
				works &= (ReadType(IntVec(c, r) - DirToVec(Direction::UP)) == CellType::UNKNOWN);

				// Block col
				if (works)
				{
					for (int rBlock = 0; rBlock < board.h; rBlock++)
					{
						if (abs(rBlock - r) > 1 && ReadType(IntVec(c, rBlock)) == CellType::UNKNOWN)
						{
							StrategyResult res = SetType(IntVec(c, rBlock), CellType::BLOCKED);
							if (res == StrategyResult::FOUND_FLAW) return StrategyResult::FOUND_FLAW;
							applicable |= (res == StrategyResult::SUCCESS);
						}
					}
				}
			}
		}
	}
	for (int r = 0; r < board.h; r++)
	{
		// Count rails in row
		int railNum = 0;
		for (int c = 0; c < board.w; c++)
		{
			if (ReadType(IntVec(c, r)) == CellType::RAIL)
			{
				railNum++;
			}
		}

		// Row is one off
		if (railNum == board.rowLabels[r] - 1)
		{
			// Loop through rails in row (c, r)
			for (int c = 0; c < board.w; c++)
			{
				if (ReadType(IntVec(c, r)) != CellType::RAIL) { continue; }

				// Count connections
				if (StateCount(c, r, State::POSSIBLE, State::POSSIBLE) != 2) { continue; }

				// Check connections are parallel
				if (ReadTrack(c, r, Direction::RIGHT) == State::POSSIBLE && ReadTrack(c, r, Direction::LEFT) == State::POSSIBLE) {}
				else { continue; }

				// Check neighboring cells are empty
				bool works = true;
				works = works && (ReadType(IntVec(c, r) + DirToVec(Direction::RIGHT)) == CellType::UNKNOWN);
				works = works && (ReadType(IntVec(c, r) - DirToVec(Direction::RIGHT)) == CellType::UNKNOWN);

				// Block row
				if (works)
				{
					for (int cBlock = 0; cBlock < board.w; cBlock++)
					{
						if (abs(cBlock - c) > 1 && ReadType(IntVec(cBlock, r)) == CellType::UNKNOWN)
						{
							StrategyResult res = SetType(IntVec(cBlock, r), CellType::BLOCKED);
							if (res == StrategyResult::FOUND_FLAW) return StrategyResult::FOUND_FLAW;
							applicable |= (res == StrategyResult::SUCCESS);
						}
					}
				}
			}
		}
	}
	if (applicable)
		return StrategyResult::SUCCESS;
	else
		return StrategyResult::UNSUITABLE;
}

StrategyResult PuzzleSolver::BImpossibleSide()
{
	Board& board = *boardPtr;
	int railNum;
	bool applicable = false;
	for (int c = 0; c < board.w; c++)
	{
		railNum = 0;
		for (int r = 0; r < board.h; r++)
		{
			if (ReadType(IntVec(c, r)) == CellType::RAIL)
			{
				railNum++;
			}
		}
		if (railNum == board.colLabels[c] - 1)
		{
			for (int r = 0; r < board.h; r++)
			{
				if (ReadType(IntVec(c, r)) == CellType::RAIL) { continue; }
				bool sideBlocked = false;
				if (ReadTrack(IntVec(c + 1, r), Direction::LEFT) == State::IMPOSSIBLE) { sideBlocked = true; }
				if (ReadTrack(IntVec(c - 1, r), Direction::RIGHT) == State::IMPOSSIBLE) { sideBlocked = true; }


				if (sideBlocked)
				{
					bool nextToRail = false;
					if (ReadType(IntVec(c, r - 1)) == CellType::RAIL && ReadTrack(IntVec(c, r - 1), Direction::DOWN) != State::IMPOSSIBLE) { nextToRail = true; }
					if (ReadType(IntVec(c, r + 1)) == CellType::RAIL && ReadTrack(IntVec(c, r + 1), Direction::UP) != State::IMPOSSIBLE) { nextToRail = true; }

					if (!nextToRail)
					{
						StrategyResult res = SetType(IntVec(c, r), CellType::BLOCKED);
						if (res == StrategyResult::FOUND_FLAW) return StrategyResult::FOUND_FLAW;
						applicable |= (res == StrategyResult::SUCCESS);
					}
				}
			}
		}
	}
	for (int r = 0; r < board.h; r++)
	{
		railNum = 0;
		for (int c = 0; c < board.w; c++)
		{
			if (ReadType(IntVec(c, r)) == CellType::RAIL)
			{
				railNum++;
			}
		}
		if (railNum == board.rowLabels[r] - 1)
		{
			for (int c = 0; c < board.w; c++)
			{
				if (ReadType(IntVec(c, r)) == CellType::RAIL) { continue; }
				bool sideBlocked = false;
				if (ReadTrack(IntVec(c, r - 1), Direction::DOWN) == State::IMPOSSIBLE) { sideBlocked = true; }
				if (ReadTrack(IntVec(c, r + 1), Direction::UP) == State::IMPOSSIBLE) { sideBlocked = true; }


				if (sideBlocked)
				{
					bool nextToRail = false;
					if (ReadType(IntVec(c - 1, r)) == CellType::RAIL && ReadTrack(IntVec(c - 1, r), Direction::RIGHT) != State::IMPOSSIBLE) { nextToRail = true; }
					if (ReadType(IntVec(c + 1, r)) == CellType::RAIL && ReadTrack(IntVec(c + 1, r), Direction::LEFT) != State::IMPOSSIBLE) { nextToRail = true; }

					if (!nextToRail)
					{
						StrategyResult res = SetType(IntVec(c, r), CellType::BLOCKED);
						if (res == StrategyResult::FOUND_FLAW) return StrategyResult::FOUND_FLAW;
						applicable |= (res == StrategyResult::SUCCESS);
					}
				}
			}
		}
	}
	if (applicable)
		return StrategyResult::SUCCESS;
	else
		return StrategyResult::UNSUITABLE;
}

StrategyResult PuzzleSolver::BDoubleImpossibleSide()
{
	Board& board = *boardPtr;

	int railNum;
	bool applicable = false;
	for (int c = 0; c < board.w; c++)
	{
		railNum = 0;
		for (int r = 0; r < board.h; r++)
		{
			if (ReadType(IntVec(c, r)) == CellType::RAIL)
			{
				railNum++;
			}
		}
		if (railNum >= board.colLabels[c] - 2)
		{
			for (int r = 0; r < board.h; r++)
			{
				if (ReadType(IntVec(c, r)) == CellType::RAIL) { continue; }
				bool sideBlocked = ReadTrack(IntVec(c + 1, r), Direction::LEFT) == State::IMPOSSIBLE
					&& ReadTrack(IntVec(c - 1, r), Direction::RIGHT) == State::IMPOSSIBLE;


				if (sideBlocked)
				{
					int nextToRail = 0;
					if (ReadType(IntVec(c, r - 1)) == CellType::RAIL && ReadTrack(IntVec(c, r - 1), Direction::DOWN) != State::IMPOSSIBLE) { nextToRail++; }
					if (ReadType(IntVec(c, r + 1)) == CellType::RAIL && ReadTrack(IntVec(c, r + 1), Direction::UP) != State::IMPOSSIBLE) { nextToRail++; }

					if ((railNum == board.colLabels[c] - 2 && nextToRail == 0) || (railNum == board.colLabels[c] - 1 && nextToRail <= 1))
					{
						StrategyResult res = SetType(IntVec(c, r), CellType::BLOCKED);
						if (res == StrategyResult::FOUND_FLAW) return StrategyResult::FOUND_FLAW;
						applicable |= (res == StrategyResult::SUCCESS);
					}
				}
			}
		}
	}
	for (int r = 0; r < board.h; r++)
	{
		railNum = 0;
		for (int c = 0; c < board.w; c++)
		{
			if (ReadType(IntVec(c, r)) == CellType::RAIL)
			{
				railNum++;
			}
		}
		if (railNum >= board.rowLabels[r] - 2)
		{
			for (int c = 0; c < board.w; c++)
			{
				if (ReadType(IntVec(c, r)) == CellType::RAIL) { continue; }
				bool sideBlocked = ReadTrack(IntVec(c, r - 1), Direction::DOWN) == State::IMPOSSIBLE
					&& ReadTrack(IntVec(c, r + 1), Direction::UP) == State::IMPOSSIBLE;

				if (sideBlocked)
				{
					int nextToRail = 0;
					if (ReadType(IntVec(c - 1, r)) == CellType::RAIL && ReadTrack(IntVec(c - 1, r), Direction::RIGHT) != State::IMPOSSIBLE) { nextToRail++; }
					if (ReadType(IntVec(c + 1, r)) == CellType::RAIL && ReadTrack(IntVec(c + 1, r), Direction::LEFT) != State::IMPOSSIBLE) { nextToRail++; }

					if ((railNum == board.rowLabels[r] - 2 && nextToRail == 0) || (railNum == board.rowLabels[r] - 1 && nextToRail <= 1))
					{
						StrategyResult res = SetType(IntVec(c, r), CellType::BLOCKED);
						if (res == StrategyResult::FOUND_FLAW) return StrategyResult::FOUND_FLAW;
						applicable |= (res == StrategyResult::SUCCESS);
					}
				}
			}
		}
	}
	if (applicable)
		return StrategyResult::SUCCESS;
	else
		return StrategyResult::UNSUITABLE;
}

StrategyResult PuzzleSolver::BClosedLoop()
{
	Board& board = *boardPtr;

	bool applicable = false;
	int pathLength = 1;
	IntVec startPos, currentPos;

	for (int r = 0; r < board.h; r++)
	{
		for (int c = 0; c < board.w; c++)
		{
			startPos = IntVec(c, r);
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
						tempBackwardsDir = Opposite((Direction)i);
					}
				}
				backwardsDir = tempBackwardsDir;

				if (pathLength == 2) { continue; }
				auto relDir = NextTo(startPos, currentPos);
				if (relDir.has_value())
				{
					StrategyResult res = SetTrack(startPos, relDir.value(), State::IMPOSSIBLE);
					if (res == StrategyResult::FOUND_FLAW) return StrategyResult::FOUND_FLAW;
					applicable |= (res == StrategyResult::SUCCESS);
				}
				if (pathLength > board.w * board.h) { break; }
				if (!board.In(currentPos)) { break; }
			}
		}
	}
	if (applicable)
		return StrategyResult::SUCCESS;
	else
		return StrategyResult::UNSUITABLE;
}