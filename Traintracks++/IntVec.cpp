#include "IntVec.h"

IntVec::IntVec()
{
	x = 0;
	y = 0;
}

IntVec::IntVec(int x_, int y_)
{
	x = x_;
	y = y_;
}

IntVec IntVec::operator+(IntVec add)
{
	return { x + add.x, y + add.y };
}

IntVec IntVec::operator-(IntVec sub)
{
	return { x - sub.x, y - sub.y };
}

bool IntVec::operator==(IntVec other)
{
	return (x == other.x) && (y == other.y);
}

bool IntVec::IsUnit()
{
	return std::abs(x) + std::abs(y) == 1;
}

void IntVec::operator+=(IntVec add)
{
	x += add.x;
	y += add.y;
}