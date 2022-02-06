#include "pch.h"

#include <GameEngine\Components\UI\Button.h>
#include <GameEngine\InputManager.h>
#include <GameEngine\Log.h>

namespace Game::UI {

	struct ConstBufferData {
		Vector4f state;
	};

	Shader* Button::button_shader{nullptr};

	void Button::OnInit() {
		auto& scene = GetSceneObject().GetScene();
		if (button_shader == nullptr) {
			button_shader = &scene.GetEngine().CreateShader();
			button_shader->LoadVertexShader(Util::ReadFile(L"VertexShader.cso"));
			button_shader->LoadPixelShader(Util::ReadFile(L"ButtonShader.cso"));
		}
		auto device = scene.GetEngine().GetDevice();
		indexBuf = device->CreateBuffer<UINT>({ 2,1,0,0,3,2 }, D3D11_BIND_INDEX_BUFFER);

		vector<Vertex> vertices = {
				{ {-1.f, -1.f, 0 } },
				{ {1.f, -1.f, 0 } },
				{ {1.f, 1.f, 0 } },
				{ {-1.f, 1.f, 0 } },
		};
		for (auto& v : vertices) {
			v.texcoord.x = v.position.x * .5f + .5f;
			v.texcoord.y = v.position.y * .5f + .5f;
		}


		vertexBuf = device->CreateBuffer<Vertex>(vertices, D3D11_BIND_VERTEX_BUFFER);
		PSConstBuffer = device->CreateBuffer<ConstBufferData>({ { Vector4f(0,0,0,0) } }, D3D11_BIND_CONSTANT_BUFFER);
	}

	void Button::Update() {
		
		if (checkHover()) {
			if (InputManager::GetKey(KeyCode::MouseLeft) && 
				(state == ButtonState::HOVER || state == ButtonState::PRESSED)) {
				setState(ButtonState::PRESSED);
			}
			else {
				setState(ButtonState::HOVER);
			}
		} else {
			if (state == ButtonState::PRESSED) {
				setState(ButtonState::HOVER);
			}
			else {
				setState(ButtonState::NEUTRAL);
			}
		}

		fireListener(on_click_listener, prev_state == ButtonState::HOVER && state == ButtonState::PRESSED);
		fireListener(on_release_listener, prev_state == ButtonState::PRESSED && state == ButtonState::HOVER);
		fireListener(on_hover_in_listener, prev_state == ButtonState::NEUTRAL && state == ButtonState::HOVER);
		fireListener(on_hover_out_listener, prev_state == ButtonState::HOVER && state == ButtonState::NEUTRAL);
	}

	void Button::Draw(Device* device) {
		auto& ren = GetSceneObject().GetScene().GetRender();
		auto buf = ren.GetConstBuffer();

		Vector2 scale, pos;
		scale = canvas->TransformScaleToView(transform->GetScale2D());
		pos = canvas->TransformPositionToView(transform->GetPosition2DWithAnchor(canvas));

		pos *= 2;
		pos -= Vector2::one;
		pos.y = -pos.y;

		scale.y = -scale.y;

		ConstantBufferData data;
		data.LightWVP = Matrix4x4f::identity;
		data.Projection = Matrix4x4f::identity;

		data.WorldView = data.World = Matrix4x4::Translation(-transform->GetAnchor()) * Matrix4x4::Scale({ scale.x, scale.y }) * Matrix4x4::Translation({ pos.x, pos.y });

		device->LoadBufferSubresource(buf, data);
		device->ClearZBuffer();
		device->SetVPDefault();
		device->SetRenderTargetsDefault();

		device->SetBlendState(true);
		device->SetRSState(false);

		device->SetActiveVertexBuffer<Vertex>(vertexBuf.Get());
		device->SetActiveIndexBuffer(indexBuf.Get());
		device->SetActiveVSConstantBuffer(buf);
		device->SetActivePSConstantBuffer(PSConstBuffer.Get());
		device->SetPSTextures({ tex });

		device->SetActiveShader(*button_shader);

		device->UseDepthBuffer(false);
		device->Draw();
	}

	bool Button::checkHover() {
		Vector2 buttonTopRight = canvas->TransformPositionToScreen(transform->GetPosition2DWithAnchor(canvas));
		Vector2 buttonScale = canvas->TransformScaleToScreen(transform->GetScale2D());
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