#include <GameFramework/Util.h>

namespace GameEngine {
	void ThrowIfFailed(HRESULT hr) {
		if (FAILED(hr))
			throw hr;
	}
}