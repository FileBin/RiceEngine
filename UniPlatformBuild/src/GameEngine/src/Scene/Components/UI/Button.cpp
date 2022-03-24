#include "pch.h"

#include <GameEngine\Components\UI\Button.h>
#include <GameEngine\InputManager.h>

namespace Game::UI {

	Shader* Button::tex_shader{ nullptr };

	void Button::OnInit() {
		SetShaderName("ButtonShaderPath");
		Image::OnInit();
		PSConstBuffer = GetSceneObject().GetScene().GetEngine().GetDevice()->
			CreateBuffer<ConstBufferData>({ { Vector4f(1,1,1,0) } }, D3D11_BIND_CONSTANT_BUFFER);
	}

	void Button::Update() {
		
		bool hover = checkHover();
		if (hover || state == ButtonState::PRESSED) {
			if (InputManager::GetKey(KeyCode::MouseLeft) &&
				(state == ButtonState::HOVER || state == ButtonState::PRESSED)) {
				setState(ButtonState::PRESSED);
			} else {
				if (hover)
					setState(ButtonState::HOVER);
				else
					setState(ButtonState::NEUTRAL);
			}
		} else {
			setState(ButtonState::NEUTRAL);
		}

		fireListener(on_click_listener, prev_state == ButtonState::HOVER && state == ButtonState::PRESSED);
		fireListener(on_release_listener, prev_state == ButtonState::PRESSED && state == ButtonState::HOVER);
		fireListener(on_hover_in_listener, prev_state == ButtonState::NEUTRAL && state == ButtonState::HOVER);
		fireListener(on_hover_out_listener, prev_state == ButtonState::HOVER && state == ButtonState::NEUTRAL);
	}

	void Button::Draw(Device* device) {
		device->SetActivePSConstantBuffer(PSConstBuffer.Get());
		Image::Draw(device);
	};

	bool Button::checkHover() {
		Vector2 buttonTopRight = canvas->TransformPositionToScreen(transform->GetPosition2DWithAnchor(canvas));
		Vector2 buttonScale = canvas->TransformScaleToScreen(transform->GetReferenceScale2D());
		Vector2 anchor = transform->GetAnchor();
		anchor += Vector2::one;
		anchor *= .5;
		buttonTopRight -= anchor * buttonScale;
		Vector2 mousePos = InputManager::GetMousePos();
		if (mousePos.x < buttonTopRight.x || mousePos.y < buttonTopRight.y)
			return false;
		auto delta = mousePos - buttonTopRight;
		if (delta.x > buttonScale.x || delta.y > buttonScale.y)
			return false;
		return true;
	}

	void Button::setState(Button::ButtonState state) {
		prev_state = this->state;
		this->state = state;
		ConstBufferData data{ {1,1,1,0} };
		switch (state) {
		case ButtonState::DISABLED:
			data.state.x = data.state.y = data.state.z = data.state.w = 1.;
			break;
		case ButtonState::NEUTRAL:
			data.state.w = 0.;
			data.state.x = neutral_color.r;
			data.state.y = neutral_color.g;
			data.state.z = neutral_color.b;
			break;
		case ButtonState::HOVER:
			data.state.w = 0.;
			data.state.x = hover_color.r;
			data.state.y = hover_color.g;
			data.state.z = hover_color.b;
			break;
		case ButtonState::PRESSED:
			data.state.w = 0.;
			data.state.x = press_color.r;
			data.state.y = press_color.g;
			data.state.z = press_color.b;
			break;
		}
		auto device = GetSceneObject().GetScene().GetEngine().GetDevice();

		device->LoadBufferSubresource(PSConstBuffer.Get(), data);
	}
}