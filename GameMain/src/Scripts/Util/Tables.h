#pragma once
#include <vector>
#include <GameEngine\Util\defines.h>
#include <GameEngine\Vectors\Vector3i.h>

class Tables {
public:
	static const Vector3i cubeVertices[8];
	static const int triangualtionTable[][16];
	static const int edges[][2];
};
