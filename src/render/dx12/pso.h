#pragma once

// Forward declarations
struct ID3D12PipelineState;

namespace angler {
    class PSO {
    public:
        PSO() = default;
        virtual ~PSO();
        
        PSO(const PSO& _pso);
        
        ID3D12PipelineState* get();
        
    protected:
        ID3D12PipelineState* m_pso = nullptr;
    };
}