#pragma once

#include "UIBase.h"

namespace Game::UI {
	class Image : public UIBase {
	public:
		void OnInit();

		void SetColor(D2D1::ColorF color) { m_color = color; }

		void SetImg(Texture2D* texture) { tex = texture; }

		void Draw(Device* dev);
	private:
		D2D1::ColorF m_color = D2D1::ColorF::Black;

		Texture2D* tex;
		Microsoft::WRL::ComPtr<Buffer> indexBuf, vertexBuf;

		static Shader* tex_shader;
	};

	inline Shader* Image::tex_shader;
}
