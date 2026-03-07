#include "pch.h"
#include "compute-pso.h"

// Local dependencies
#include "../graphics/graphics.h"
#include "../root-signatures/compute-root-sig.h"
 
namespace angler {
    void ComputePSO::initialize(const ComputeRootSig* _rootSig, IDxcBlob* _cs) {
        D3D12_COMPUTE_PIPELINE_STATE_DESC computePSODesc = {};
        computePSODesc.pRootSignature = _rootSig->get();
        computePSODesc.CS = {
            static_cast<BYTE*>(_cs->GetBufferPointer()),
            _cs->GetBufferSize()
        };
        computePSODesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
        
        ThrowIfFailed(Graphics::getDevice()->CreateComputePipelineState(&computePSODesc, IID_PPV_ARGS(&m_pso)));
    }
}