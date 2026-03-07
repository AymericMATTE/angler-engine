#pragma once

// Local dependencies
#include "common/types.h"
#include "../buffers/buffer-interface.h"

// External dependencies
#include <d3d12.h>

namespace angler {
    class Texture final : public ISRVResource {
    public:
        Texture() = default;
        Texture(const Texture& _other);
        Texture(Texture&& _other) noexcept;
        Texture& operator=(const Texture& _other);
        Texture& operator=(Texture&& _other) noexcept;

        ~Texture();

        void loadFromDDS(const wchar_t* _filePath);
        
        void transition(D3D12_RESOURCE_STATES _newState);

        [[nodiscard]] ID3D12Resource* getResource() const override;
        [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS getGPUAddress() const override;
        [[nodiscard]] DXGI_FORMAT getFormat() const;
        [[nodiscard]] uint getWidth() const override;
        [[nodiscard]] uint getHeight() const override;

    private:
        ID3D12Resource* m_resource = nullptr;

        uint m_width = 0;
        uint m_height = 0;

        D3D12_RESOURCE_STATES m_currentState = D3D12_RESOURCE_STATE_COMMON;

        friend class GraphicsResourceManager;
    };
}