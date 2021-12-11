#pragma once
#include "stdafx.h"
#include <xnamath.h>
#include "Vectors.h"

typedef std::vector<byte> data_t;
typedef std::wstring String;
typedef std::vector<D3D11_INPUT_ELEMENT_DESC> VertexLayout;
typedef ID3D11Buffer Buffer;

template<typename T> struct List : public std::vector<T> {};
template<typename K, typename V> struct Map : public std::map<K, V> {};

namespace GameEngine::Util {

	void ThrowIfFailed(HRESULT hr);
	data_t ReadFile(String filename);
	String Utf8ToWstring(const std::string& str);

	struct Color {
		float r, g, b, a;
		Color(float red, float green, float blue) : r(red), g(green), b(blue), a(1) {}
		Color(float red, float green, float blue, float alpha) : r(red), g(green), b(blue), a(alpha) {}
	};

	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT4 tangents;
		XMFLOAT2 texcoord;

		static VertexLayout GetLayout() {
			return {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
		}
	};

	struct ConstantBufferData {
		XMMATRIX World, View, Projection;
	};

	struct Transform {
		Vector3 pos;
		Quaternion rotation;
		Vector3 scale;

		XMMATRIX GetTransformationMatrix();
	};
}