#include "pch.h"
#include "graphics-root-sig.h"

// Local dependencies
#include "../graphics/graphics.h"

namespace angler {
	GraphicsRootSig::GraphicsRootSig(){
	}

	void GraphicsRootSig::initialize()
	{
		uint parametersSize = static_cast<uint>(m_parameters.size());
		m_parameters.resize(parametersSize + 3);
		m_parameters[parametersSize].InitAsConstantBufferView(0);
		m_parameters[parametersSize + 1].InitAsDescriptorTable(1, Graphics::getSRVRange());
		m_parameters[parametersSize + 2].InitAsDescriptorTable(1, Graphics::getSamplerRange());

		RootSig::initialize();
	}

	void GraphicsRootSig::addConstantBufferView(uint _shaderRegister) {
		CD3DX12_ROOT_PARAMETER param;
		param.InitAsConstantBufferView(_shaderRegister, 1);
		m_parameters.push_back(param);
	}

	void GraphicsRootSig::addUnorderedAccessView(uint _shaderRegister) {
		CD3DX12_ROOT_PARAMETER param;
		param.InitAsUnorderedAccessView(_shaderRegister, 1);
		m_parameters.push_back(param);
	}

	void GraphicsRootSig::addShaderResourceView(uint _shaderRegister) {
		CD3DX12_ROOT_PARAMETER param;
		param.InitAsShaderResourceView(_shaderRegister, 1);
		m_parameters.push_back(param);
	}

	uint GraphicsRootSig::getFixedParameterCount() const {
		return 3;
	}
}