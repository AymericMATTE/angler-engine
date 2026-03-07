#include "pch.h"
#include "pso.h"

namespace angler {
    PSO::~PSO() {
        if (m_pso) {
            m_pso->Release();
            m_pso = nullptr;
        }
    }
    PSO::PSO(const PSO& _pso) {
        if (m_pso)
            m_pso->AddRef();
    }
    ID3D12PipelineState* PSO::get() { return m_pso; }
}