#include "pch.h"
#include "shader.h"

// Local dependencies
#include "../graphics/graphics.h"

namespace angler {
	void Shader::initialize(wcstr _vs, wcstr _ps, const GraphicsRootSig* _rootSig, const InputLayout* _inputLayout) {
		m_rootSignature = *_rootSig;
		m_inputLayout = *_inputLayout;

		Graphics::compileShader(_vs, L"vs_6_0", L"VS", &m_compiledVS);
		Graphics::compileShader(_ps, L"ps_6_0", L"PS", &m_compiledPS);

		IDxcBlob* resultVS;
		IDxcBlob* resultPS;

		m_compiledVS->GetResult(&resultVS);
		m_compiledPS->GetResult(&resultPS);

		m_opaque.initialize(_rootSig, _inputLayout, resultVS, resultPS,
			Graphics::getRenderTargetFormat(), Graphics::getDepthStencilFormat(), false, -1);
		
		m_alpha.initialize(_rootSig, _inputLayout, resultVS, resultPS,
			Graphics::getRenderTargetFormat(), Graphics::getDepthStencilFormat(), true, -1);

		resultVS->Release();
		resultPS->Release();
	}

	void Shader::setAlpha(const bool _alphaEnabled) {
		m_isAlphaEnabled = _alphaEnabled;
	}

	bool Shader::isAlphaEnabled() const {
		return m_isAlphaEnabled;
	}

	GraphicsPSO* Shader::getPSO() {
		return m_isAlphaEnabled ? &m_alpha : &m_opaque;
	}

	GraphicsRootSig* Shader::getRootSignature() {
		return &m_rootSignature;
	}

	InputLayout* Shader::getInputLayout() {
		return &m_inputLayout;
	}

	Shader::~Shader()
	{
		m_compiledPS->Release();
		m_compiledVS->Release();
	}
	
	IDxcResult* Shader::getVertexResult() const {
		return m_compiledVS;
	}
	IDxcResult* Shader::getPixelResult() const {
		return m_compiledPS;
	}
}