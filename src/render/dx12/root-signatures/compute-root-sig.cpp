#include "pch.h"
#include "compute-root-sig.h"

// Local dependencies
#include "../graphics/graphics.h"

namespace angler {
	void ComputeRootSig::addConstantBufferView(uint _shaderRegister) {
		CD3DX12_ROOT_PARAMETER param;
		param.InitAsConstantBufferView(_shaderRegister);
		m_parameters.push_back(param);
	}

	void ComputeRootSig::addUnorderedAccessView(uint _shaderRegister) {
		CD3DX12_ROOT_PARAMETER param;
		param.InitAsUnorderedAccessView(_shaderRegister);
		m_parameters.push_back(param);
	}

	void ComputeRootSig::addShaderResourceView(uint _shaderRegister) {
		CD3DX12_ROOT_PARAMETER param;
		param.InitAsShaderResourceView(_shaderRegister);
		m_parameters.push_back(param);
	}
}