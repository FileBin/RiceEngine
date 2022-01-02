#pragma once
#include "../stdafx.h"
#include "../Util/defines.h"
#include "../macros.h"
#include "../Util.h"

namespace Game {
	class Texture2D {
	public:
		Texture2D(String filename, ID3D11Device* device);

		ID3D11SamplerState* GetSampler() { return pSampler; }
		ID3D11ShaderResourceView* GetResource() { return pResource; }

		~Texture2D();
	private:
		ID3D11SamplerState* pSampler;
		ID3D11ShaderResourceView* pResource;
	};
}
