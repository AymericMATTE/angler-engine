#include "pch.h"
#include "root-sig.h"

// Local dependencies
#include "graphics/graphics.h"

namespace angler {
	RootSig::RootSig(const RootSig& _other) {
		m_rootSignature = _other.m_rootSignature;
		m_parameters = _other.m_parameters;
		if(m_rootSignature)
			m_rootSignature->AddRef();
	}

	RootSig& RootSig::operator=(const RootSig& _other) {
		DX12_OBJ_RELEASE(m_rootSignature)
		
		m_rootSignature = _other.m_rootSignature;
		m_parameters = _other.m_parameters;
		if (m_rootSignature)
			m_rootSignature->AddRef();

		return *this;
	}

	RootSig& RootSig::operator=(RootSig&& _other) noexcept {
		DX12_OBJ_RELEASE(m_rootSignature)
		
		m_rootSignature = _other.m_rootSignature;
		m_parameters = _other.m_parameters;
		if (m_rootSignature)
			m_rootSignature->AddRef();

		return *this;
	}

	RootSig::RootSig(RootSig&& _other) noexcept {
		m_rootSignature = _other.m_rootSignature;
		m_parameters = _other.m_parameters;
		if (m_rootSignature)
			m_rootSignature->AddRef();
	}

	void RootSig::initialize() { // Call after adding parameters
		ID3D12Device* d3dDevice = Graphics::getDevice();

		CD3DX12_ROOT_SIGNATURE_DESC desc((uint)m_parameters.size(), m_parameters.data(), 0,
			nullptr,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ID3DBlob* serializedRootSig = nullptr;
		ID3DBlob* errors = nullptr;
		ThrowIfFailed(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSig, &errors))

		ThrowIfFailed(d3dDevice->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)))
	}

	ID3D12RootSignature* RootSig::get() const {
		return m_rootSignature;
	}

	uint RootSig::getParameterCount() const {
		return static_cast<uint>(m_parameters.size());
	}

	std::vector<CD3DX12_ROOT_PARAMETER>& RootSig::getParameters() {
		return m_parameters;
	}

	RootSig::~RootSig(){
		DX12_OBJ_RELEASE(m_rootSignature)
	}
}