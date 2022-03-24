#pragma once
#include "Vectors.h"
#include "Matrixes.h"
#include "stdafx.h"
#include "Util.h"
#include "Vectors/Quaternion.h"

namespace Game {

	struct Vertex {
		Vector3f position = { 0,0,0 };
		Vector3f normal = { 0,0,0 };
		Vector4f tangents = { 0,0,0,0 };
		Vector2f texcoord = { 0,0 };

		static VertexLayout GetLayout() {
			return {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
		}
	};

	struct ConstantBufferData {
		Matrix4x4f World = Matrix4x4f::identity, WorldView = Matrix4x4f::identity, Projection = Matrix4x4f::identity, LightWVP = Matrix4x4f::identity;
	};
}
