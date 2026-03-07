#pragma once

// Local dependencies
#include "common/types.h"
#include "../pipeline-states/compute-pso.h"

// Forward declarations
namespace angler {
    class ComputeRootSig;
}

namespace angler {
    class ComputeShader {
    public:
        void initialize(wcstr _cs, ComputeRootSig* _rootSig);
        ComputePSO* getPSO() const;
        ComputeRootSig* getRootSignature() const;
    private:
        mutable ComputePSO m_compute;
        ComputeRootSig* m_computeRootSig = nullptr;
    };
}