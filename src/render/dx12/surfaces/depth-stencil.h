#pragma once

// Local dependencies
#include "common/types.h"
#include "../utils/d3dx12.h"
#include "../buffers/buffer-interface.h"

namespace angler {
    class DepthStencil final : public IGraphicsResource {
    public:
        DepthStencil() = default;
        DepthStencil(const DepthStencil& _other) = delete;
        DepthStencil(DepthStencil&& _other) = delete;
        DepthStencil& operator=(const DepthStencil& _other) = delete;
        DepthStencil& operator=(DepthStencil&& _other) = delete;

        ~DepthStencil() override;
        
        // TO DO : move MSAA DepthStencil init to a separate function
        void initialize(uint _width, uint _height, 
            uint _sampleCount = 1, uint _sampleQuality = 0,
            D3D12_RESOURCE_DIMENSION _dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
            uint _mipLevels = 1);

        [[nodiscard]] ID3D12Resource* getResource() const override { return m_buffer; }
        [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS getGPUAddress() const override;
        [[nodiscard]] CD3DX12_CPU_DESCRIPTOR_HANDLE getDepthStencilView() const;
        [[nodiscard]] CD3DX12_GPU_DESCRIPTOR_HANDLE getGPUDescriptorHandle() const;
		
        // MSAA related getters
        [[nodiscard]] bool isMSAA() const { return m_sampleCount > 1; }
        [[nodiscard]] uint getSampleCount() const { return m_sampleCount; }
        [[nodiscard]] uint getSampleQuality() const { return m_sampleQuality; }

    private:
        ID3D12DescriptorHeap* m_viewHeap = nullptr;
        ID3D12Resource* m_buffer = nullptr;
		
        uint m_sampleCount = 1;
        uint m_sampleQuality = 0;
		
        friend class Graphics;
    };
}