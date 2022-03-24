#include "pch.h"

#include <GameEngine\Components\UI\Slider.h>
#include <GameEngine\InputManager.h>
#include <GameEngine\Log.h>

namespace Game::UI {
	Shader* Slider::tex_shader{ nullptr };

	void Slider::OnInit() {
		SetShaderName("SliderShaderPath");
		Image::OnInit();
		PSConstBuffer = GetSceneObject().GetScene().GetEngine().GetDevice()->
			CreateBuffer<ConstBufferData>({ { Vector4f(1,1,1,0), {0} } }, D3D11_BIND_CONSTANT_BUFFER);
	}

	void Slider::SetProgress(dbl progress, bool notify_listener) {
		progress = Math::Clamp01(progress);
		prev_progress = this->progress;
		this->progress = progress;
		fireListener(on_progress_changed_listener, notify_listener && prev_progress != progress);
	}

	void Slider::Draw(Device* device) {
		auto& ren = GetSceneObject().GetScene().GetRender();
		auto buf = ren.GetConstBuffer();

		Vector2 scale, pos, scale_bg, pos_root;

		Vector2 scale_raw = transform->GetReferenceScale2D();

		scale = canvas->TransformScaleToView({ scale_raw.y, scale_raw.y });
		scale_bg = canvas->TransformScaleToView(scale_raw);
		pos = canvas->TransformPositionToView(getPosition(progress));
		pos *= 2;
		pos -= Vector2::one;
		pos.y = -pos.y;

		pos_root = canvas->TransformPositionToView(getPosition(0.5));

		pos_root *= 2;
		pos_root -= Vector2::one;
		pos_root.y = -pos_root.y;

		//scale_bg.x /= ren.GetAspectRatio();

		scale.y = -scale.y;
		scale_bg.y = -scale_bg.y;

		ConstantBufferData data;
		data.LightWVP = Matrix4x4f::identity;
		data.Projection = Matrix4x4f::identity;

		device->ClearZBuffer();
		device->SetVPDefault();
		device->SetRenderTargetsDefault();

		device->SetBlendState(true);
		device->SetRSState(false);

		device->SetActiveVertexBuffer<Vertex>(vertexBuf.Get());
		device->SetActiveIndexBuffer(indexBuf.Get());
		device->SetActiveVSConstantBuffer(buf);
		device->SetActivePSConstantBuffer(PSConstBuffer.Get());
		
		device->SetActiveShader(*tex_shader);
		device->UseDepthBuffer(false);

		data.WorldView = data.World = Matrix4x4::Translation(-transform->GetAnchor()) * Matrix4x4::Scale(scale_bg) * Matrix4x4::Translation(pos_root);
		device->LoadBufferSubresource(buf, data);

		device->SetPSTextures({ bg_tex });

		device->Draw();

		/*data.WorldView = data.World = Matrix4x4::Translation(-transform->GetAnchor()) * Matrix4x4::Scale(scale) * Matrix4x4::Translation(pos);
		device->LoadBufferSubresource(buf, data);

		device->SetPSTextures({ tex });

		device->Draw();*/
	}

	Vector2 Slider::getPosition(dbl progr) {
		auto& ren = GetSceneObject().GetScene().GetRender();
		return transform->GetPosition2DWithAnchor(canvas) + direction * (progr - .5) * transform->GetScale2DTransformed(ren.GetAspectRatio());
	}

	bool Slider::checkHover() {
		Vector2 topRight = canvas->TransformPositionToScreen(getPosition(0.5));
		Vector2 scale = canvas->TransformScaleToScreen(transform->GetReferenceScale2D());
		Vector2 anchor = transform->GetAnchor();
		anchor += Vector2::one;
		anchor *= .5;
		topRight -= anchor * scale;
		Vector2 mousePos = InputManager::GetMousePos();
		if (mousePos.x < topRight.x || mousePos.y < topRight.y)
			return false;
		auto delta = mousePos - topRight;
		if (delta.x > scale.x || delta.y > scale.y)
			return false;
		return true;
	}

	void Slider::setState(Slider::SliderState state) {
		prev_state = this->state;
		this->state = state;
		ConstBufferData data{ {1,1,1,0} };
		data.value.x = progress;
		switch (state) {
		case SliderState::DISABLED:
			data.state.x = data.state.y = data.state.z = data.state.w = 1.;
			break;
		case SliderState::NEUTRAL:
			data.state.w = 0.;
			data.state.x = neutral_color.r;
			data.state.y = neutral_color.g;
			data.state.z = neutral_color.b;
			break;
		case SliderState::HOVER:
			data.state.w = 0.;
			data.state.x = hover_color.r;
			data.state.y = hover_color.g;
			data.state.z = hover_color.b;
			break;
		case SliderState::PRESSED:
			data.state.w = 0.;
			data.state.x = press_color.r;
			data.state.y = press_color.g;
			data.state.z = press_color.b;
			break;
		}
		auto device = GetSceneObject().GetScene().GetEngine().GetDevice();

		device->LoadBufferSubresource(PSConstBuffer.Get(), data);
	}

	void Slider::Update() {
		bool hover = checkHover();
		if (hover || state == SliderState::PRESSED) {
			if (InputManager::GetKey(KeyCode::MouseLeft) &&
				(state == SliderState::HOVER || state == SliderState::PRESSED)) {
				setState(SliderState::PRESSED);
			} else {
				if (hover)
					setState(SliderState::HOVER);
				else
					setState(SliderState::NEUTRAL);
			}
		} else {
			setState(SliderState::NEUTRAL);
		}

		fireListener(on_click_listener, prev_state == SliderState::HOVER && state == SliderState::PRESSED);
		fireListener(on_release_listener, prev_state == SliderState::PRESSED && state == SliderState::HOVER);
		fireListener(on_hover_in_listener, prev_state == SliderState::NEUTRAL && state == SliderState::HOVER);
		fireListener(on_hover_out_listener, prev_state == SliderState::HOVER && state == SliderState::NEUTRAL);

		if (state == SliderState::PRESSED) {

			auto screenPos = canvas->TransformPositionToScreen(transform->GetPosition2DWithAnchor(canvas));
			auto len = canvas->TransformScaleToScreen(transform->GetReferenceScale2D()).x;
			SetProgress(Vector2::Dot(direction, (InputManager::GetMousePos() - screenPos) ) / len + .5, true);
		}
	}

}