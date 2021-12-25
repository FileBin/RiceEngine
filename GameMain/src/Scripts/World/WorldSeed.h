#pragma once

#include <vector>

using std::vector;

struct WorldSeed {
	vector<int> heightMapSeeds;
	vector<int> CaveSeeds;

	static WorldSeed Default() {
		return{ { 1, 2, 3, 4, 5, 6, 7, 8 }, 
			{ 9, 10, 11, 12 } };
	}
};
