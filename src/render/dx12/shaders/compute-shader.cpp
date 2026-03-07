#include "pch.h"
#include "compute-shader.h"

// Local dependencies
#include "../graphics/graphics.h"

namespace angler {
    void ComputeShader::initialize(wcstr _cs, ComputeRootSig* _rootSig) {
        
        m_computeRootSig = _rootSig;

        IDxcResult* compiledCS;
        Graphics::compileShader(_cs, L"cs_6_0", L"CS", &compiledCS);

        IDxcBlob* result;
        compiledCS->GetResult(&result);

        m_compute.initialize(_rootSig, result);
    }
    
    ComputePSO* ComputeShader::getPSO() const {
        return &m_compute;
    }

    ComputeRootSig* ComputeShader::getRootSignature() const {
        return m_computeRootSig;
    }
}
