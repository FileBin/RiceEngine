#include <GameFramework/Util.h>

namespace GameFramework {
	void ThrowIfFailed(HRESULT hr) {
		if (FAILED(hr))
			throw hr;
	}
}