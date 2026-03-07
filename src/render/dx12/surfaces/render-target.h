#pragma once

// Local dependencies
#include "common/types.h"
#include "../utils/d3dx12.h"
#include "../buffers/buffer-interface.h"

namespace angler {
    class RenderTarget final : public ISRVResource {
    public:
        RenderTarget() = default;
        RenderTarget(const RenderTarget& _other) = delete;
        RenderTarget(RenderTarget&& _other) = delete;
        RenderTarget& operator=(const RenderTarget& _other) = delete;
        RenderTarget& operator=(RenderTarget&& _other) = delete;

        ~RenderTarget() override;

        // Non-MSAA initialization
        void initialize(uint _width, uint _height, float _clearColor[4] = nullptr);
        void initialize(ID3D12Resource* _resource, uint _width, uint _height, float _clearColor[4] = nullptr);
		
        // MSAA initialization
        void initializeMSAA(uint _width, uint _height, uint _sampleCount, uint _quality, float _clearColor[4] = nullptr);

        [[nodiscard]] ID3D12Resource* getResource() const override;
        [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS getGPUAddress() const override;
        [[nodiscard]] uint getWidth() const override;
        [[nodiscard]] uint getHeight() const override;

        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getRTVCPUHandle() const;
        [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE getRTVGPUHandle() const;

        [[nodiscard]] D3D12_SHADER_RESOURCE_VIEW_DESC getSRVDesc() const;
		
        // MSAA related getters
        [[nodiscard]] bool isMSAA() const;
        [[nodiscard]] uint getSampleCount() const;
        [[nodiscard]] uint getSampleQuality() const;

    private :
        ID3D12Resource* m_buffer = nullptr;
        ID3D12DescriptorHeap* m_viewHeap = nullptr;

        D3D12_VIEWPORT m_viewPort = D3D12_VIEWPORT();
        D3D12_RECT m_scissorRect = D3D12_RECT();

        float m_clearColor[4] = {0,0,1,1};

        uint m_sampleCount = 1;
        uint m_sampleQuality = 0;
		
        friend class SwapChain;
        friend class Graphics;
    };
}