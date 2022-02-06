#pragma once

#include "UIBase.h"

namespace Game::UI {
	class Image : public UIBase {
	public:
		virtual void OnInit();

		void SetColor(D2D1::ColorF color) { m_color = color; }

		void SetImg(Texture2D* texture) { tex = texture; }

		virtual void Draw(Device* dev);

	protected:
		D2D1::ColorF m_color = D2D1::ColorF::White;

		Texture2D* tex;
		Microsoft::WRL::ComPtr<Buffer> indexBuf, vertexBuf;

		static Shader* tex_shader;
		
		std::string shader_name = "ImgShaderPath";

		void SetShaderName(std::string name) {
			shader_name = name;
		};

	};

	inline Shader* Image::tex_shader;
}
