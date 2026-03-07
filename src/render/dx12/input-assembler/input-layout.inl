#include "pch.h"
#include "input-layout.h"

// External dependencies
#include <DirectXMath.h>

namespace angler {
	template<typename T>
	inline void InputLayout::add(cstr _semanticName, D3D12_INPUT_CLASSIFICATION _classification) {
		// error this type is not supported !!
	}

	template <>
	inline void InputLayout::add<DirectX::XMFLOAT2>(const cstr _semanticName, D3D12_INPUT_CLASSIFICATION _classification) {
		add(_semanticName, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 8, _classification);
	}

	template <>
	inline void InputLayout::add<DirectX::XMFLOAT3>(const cstr _semanticName, D3D12_INPUT_CLASSIFICATION _classification) {
		add(_semanticName, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 12, _classification);
	}

	template <>
	inline void InputLayout::add<DirectX::XMFLOAT4>(const cstr _semanticName, D3D12_INPUT_CLASSIFICATION _classification) {
		add(_semanticName, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 16, _classification);
	}

	template <>
	inline void InputLayout::add<DirectX::XMINT3>(const cstr _semanticName, D3D12_INPUT_CLASSIFICATION _classification) {
		add(_semanticName, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_SINT, 12, _classification);
	}
}