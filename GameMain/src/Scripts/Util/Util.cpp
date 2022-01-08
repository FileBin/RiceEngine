#include "pch.h"
#include "Util.h"
#include <GameEngine\Math.h>

namespace Utilites {
	dbl Cosine(dbl x) { return -cos(x * Game::Math::PI) * .5 + .5; }
}