#pragma once

// Local dependencies
#include "../pso.h"

//External dependencies
#include <dxcapi.h>

// Forward declarations
namespace angler {
    class ComputeRootSig;
}

namespace angler {
    class ComputePSO : public PSO {
    public:
        void initialize(const ComputeRootSig* _rootSig, IDxcBlob* _cs);
    };
}