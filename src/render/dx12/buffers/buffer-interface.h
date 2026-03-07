#pragma once

// Local dependencies
#include "../utils/d3dx12.h"
#include "common/types.h"

namespace angler {
    class SRVDescriptorHeap;
}

namespace angler {
    class IGraphicsResource {
    public:
        IGraphicsResource() = default;
    
        IGraphicsResource(const IGraphicsResource& _other) = delete;
        IGraphicsResource(IGraphicsResource&& _other) noexcept = delete;
        IGraphicsResource& operator=(const IGraphicsResource& _other) = delete;
        IGraphicsResource& operator=(IGraphicsResource&& _other) noexcept = delete;
    
        [[nodiscard]] virtual ID3D12Resource* getResource() const = 0;
        [[nodiscard]] virtual D3D12_GPU_VIRTUAL_ADDRESS getGPUAddress() const = 0;

        virtual ~IGraphicsResource() = default;
    };

    class IGraphicsBuffer : public IGraphicsResource {
    public:
        IGraphicsBuffer() = default;
    
        IGraphicsBuffer(const IGraphicsBuffer& _other) = delete;
        IGraphicsBuffer(IGraphicsBuffer&& _other) noexcept = delete;
        IGraphicsBuffer& operator=(const IGraphicsBuffer& _other) = delete;
        IGraphicsBuffer& operator=(IGraphicsBuffer&& _other) noexcept = delete;
        
        [[nodiscard]] virtual uint getSize() const = 0;
    
        virtual ~IGraphicsBuffer() override = default;
    };

    class ISRVResource : public IGraphicsResource {
    public:
        ISRVResource() = default;

        ISRVResource(const ISRVResource& _other) = delete;
        ISRVResource(ISRVResource&& _other) noexcept = delete;
        ISRVResource& operator=(const ISRVResource& _other) = delete;
        ISRVResource& operator=(ISRVResource&& _other) noexcept = delete;

        [[nodiscard]] virtual uint getWidth() const = 0;
        [[nodiscard]] virtual uint getHeight() const = 0;

        [[nodiscard]] D3D12_SHADER_RESOURCE_VIEW_DESC getSRVDesc() const { return m_desc; }
        [[nodiscard]] CD3DX12_CPU_DESCRIPTOR_HANDLE getSRVCPUHandle() const;
        [[nodiscard]] CD3DX12_GPU_DESCRIPTOR_HANDLE getSRVGPUHandle() const;
        [[nodiscard]] uint getId() { return m_id; }

        virtual ~ISRVResource() override;
    protected :
        uint m_id = 0;

        SRVDescriptorHeap* m_table = nullptr;
        D3D12_SHADER_RESOURCE_VIEW_DESC m_desc = {};

        friend class SRVDescriptorHeap;
    };
}