#include "pch.h"

#include <GameEngine\Components\UI\Image.h>
#include <GameEngine\Scene\SceneObject.h>
#include <GameEngine\Scene\Scene.h>

namespace Game::UI {
	void Image::OnInit() {
		auto& scene = GetSceneObject().GetScene();
		auto& resManager = scene.GetResourceManager();
		if (tex_shader == nullptr) {
			tex_shader = &scene.GetEngine().CreateShader();
			tex_shader->LoadVertexShader(Util::ReadFile(resManager.GetString("VertexShaderPath")));
			tex_shader->LoadPixelShader(Util::ReadFile(resManager.GetString(shader_name)));
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
	}

	void Image::Draw(Device* device) {
		auto& ren = GetSceneObject().GetScene().GetRender();
		auto buf = ren.GetConstBuffer();

		Vector2 scale, pos;
		scale = canvas->TransformScaleToView(transform->GetReferenceScale2D());
		pos = canvas->TransformPositionToView(transform->GetPosition2DWithAnchor(canvas));

		pos *= 2;
		pos -= Vector2::one;
		pos.y = -pos.y;

		scale.y = -scale.y;

		ConstantBufferData data;
		data.LightWVP = Matrix4x4f::identity;
		data.Projection = Matrix4x4f::identity;

		data.WorldView = data.World = Matrix4x4::Translation(-transform->GetAnchor()) * Matrix4x4::Scale(scale) * Matrix4x4::Translation(pos);

		device->LoadBufferSubresource(buf, data);
		device->ClearZBuffer();
		device->SetVPDefault();
		device->SetRenderTargetsDefault();

		device->SetBlendState(true);
		device->SetRSState(false);

		device->SetActiveVertexBuffer<Vertex>(vertexBuf.Get());
		device->SetActiveIndexBuffer(indexBuf.Get());
		device->SetActiveVSConstantBuffer(buf);
		//device->SetActivePSConstantBuffer(mat->GetBuffer());
		device->SetPSTextures({ tex });

		device->SetActiveShader(*tex_shader);

		device->UseDepthBuffer(false);
		device->Draw();
		//dev->set
	}
}