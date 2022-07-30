#pragma once
#include <cmath>

struct IntVec
{
	IntVec();
	IntVec(int x_, int y_);
	int x, y;

	bool IsUnit();

	void operator+=(IntVec add);
	IntVec operator+(IntVec add);
	IntVec operator-(IntVec sub);
	bool operator==(IntVec other);
};