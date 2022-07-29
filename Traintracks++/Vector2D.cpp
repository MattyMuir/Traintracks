#include "Vector2D.h"

Vector2D::Vector2D()
{
	x = 0;
	y = 0;
}

Vector2D::Vector2D(int x_, int y_)
{
	x = x_;
	y = y_;
}

Vector2D Vector2D::operator+(Vector2D add)
{
	return Vector2D(x + add.x, y + add.y);
}

Vector2D Vector2D::operator-(Vector2D sub)
{
	return Vector2D(x - sub.x, y - sub.y);
}

bool Vector2D::operator==(Vector2D other)
{
	return (x == other.x) && (y == other.y);
}

bool Vector2D::IsUnit()
{
	return std::abs(x) + std::abs(y) == 1;
}

void Vector2D::operator+=(Vector2D add)
{
	x += add.x;
	y += add.y;
}