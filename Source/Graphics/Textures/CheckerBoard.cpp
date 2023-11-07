#include "CheckerBoard.h"

CheckerBoard::CheckerBoard(vec3 colorA, vec3 colorB, float scale) :
	colorA(colorA), colorB(colorB), scale(scale) { }

vec3 CheckerBoard::Sample(const HitRecord& record)
{
	int x = (record.HitPoint.x + 500.0f) / scale;
	int y = (record.HitPoint.y + 500.0f) / scale;
	int z = (record.HitPoint.z + 500.0f) / scale;

	int result = (x + y + z) % 2;

	return result ? colorA : colorB;
}