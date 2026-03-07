#include "pch.h"
#include "input-layout.h"

namespace angler {
	void InputLayout::add(const cstr _inputName, const DXGI_FORMAT _format, const uint _offsetSize, const D3D12_INPUT_CLASSIFICATION _classification) {
		D3D12_INPUT_ELEMENT_DESC desc;
		desc.SemanticName = _inputName;
		desc.SemanticIndex = 0;

		desc.Format = _format;
		desc.AlignedByteOffset = m_currentOffset;
		m_currentOffset += _offsetSize;

		desc.InputSlot = 0;
		desc.InputSlotClass = _classification;
		desc.InstanceDataStepRate = 0;

		m_inputLayout.push_back(desc);
	}
}