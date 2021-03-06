#pragma once
class Vector2D
{
public:
	Vector2D();
	Vector2D(int x_, int y_);
	int x, y;

	void operator+=(Vector2D add);
	Vector2D operator+(Vector2D add);
	Vector2D operator-(Vector2D sub);
	bool operator==(Vector2D other);
};