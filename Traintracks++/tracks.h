#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include "enums.h"
#include "Vector2D.h"
#include "Cell.h"

#define InsideGrid(cellPos) (cellPos.x >= 0 && cellPos.x < gridW && cellPos.y >= 0 && cellPos.y < gridH)