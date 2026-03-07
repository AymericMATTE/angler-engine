#pragma once

// Local dependencies
#include "common/types.h"

// External dependencies
#include <vector>

namespace angler {
	class InputLayout final {
	public:
		template <typename T>
		void add(cstr _inputName, D3D12_INPUT_CLASSIFICATION _classification = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
		[[nodiscard]] std::vector<D3D12_INPUT_ELEMENT_DESC> const* get() const { return &m_inputLayout; }

	private:
		void add(cstr _inputName, DXGI_FORMAT _format, uint _offsetSize, D3D12_INPUT_CLASSIFICATION _classification);

		std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;
		uint m_currentOffset = 0;
	};
}

#include "input-layout.inl"