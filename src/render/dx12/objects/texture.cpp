#include "pch.h"
#include "texture.h"

// Local dependencies
#include "../graphics/graphics.h"
#include "../utils/dds-loader.h"

// External dependencies
#include <wrl.h>
#include <cassert>

using Microsoft::WRL::ComPtr;

namespace angler {
    Texture::Texture(const Texture& _other) {
        m_width = _other.m_width;
        m_height = _other.m_height;
        m_desc = _other.m_desc;
        m_id = _other.m_id;
        m_currentState = _other.m_currentState;
        m_resource = _other.m_resource;
        if (m_resource) {
            m_resource->AddRef();
        }
    }

    Texture::Texture(Texture&& _other) noexcept {
        m_width = _other.m_width;
        m_height = _other.m_height;
        m_desc = _other.m_desc;
        m_id = _other.m_id;
        m_currentState = _other.m_currentState;
        m_resource = _other.m_resource;
        if (m_resource) {
            m_resource->AddRef();
        }
    }

    Texture& Texture::operator=(const Texture& _other) {
        DX12_OBJ_RELEASE(m_resource)

        m_width = _other.m_width;
        m_height = _other.m_height;
        m_desc = _other.m_desc;
        m_id = _other.m_id;
        m_currentState = _other.m_currentState;
        m_resource = _other.m_resource;
        if (m_resource) {
            m_resource->AddRef();
        }

        return *this;
    }
    
    Texture& Texture::operator=(Texture&& _other) noexcept {
        DX12_OBJ_RELEASE(m_resource)

        m_width = _other.m_width;
        m_height = _other.m_height;
        m_desc = _other.m_desc;
        m_id = _other.m_id;
        m_currentState = _other.m_currentState;
        m_resource = _other.m_resource;
        if (m_resource) {
            m_resource->AddRef();
        }

        return *this;
    }

    void Texture::loadFromDDS(const wchar_t* _filePath) {
        assert(Graphics::isDrawingStarted() == false);

        ComPtr<ID3D12Resource> texture;
        ComPtr<ID3D12Resource> uploadHeap;

        Graphics::resetCommandList();

        ThrowIfFailed(
            DirectX::CreateDDSTextureFromFile12(
                Graphics::getDevice(),
                Graphics::getGraphicsCommandList(),
                _filePath,
                texture,
                uploadHeap
            )
        )

        // Store texture resource
        m_resource = texture.Detach();

        // Get texture description info
        const auto desc = m_resource->GetDesc();
        m_width = static_cast<UINT>(desc.Width);
        m_height = desc.Height;
        m_currentState = D3D12_RESOURCE_STATE_COPY_DEST;

        m_desc = {};
        m_desc.Format = desc.Format;
        m_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        m_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        m_desc.Texture2D.MipLevels = desc.MipLevels;
        m_desc.Texture2D.MostDetailedMip = 0;

        // Finish copy
        Graphics::executeCommandList();
    }
    
    void Texture::transition(const D3D12_RESOURCE_STATES _newState) {
        if (_newState == m_currentState)
            return;

        const CD3DX12_RESOURCE_BARRIER barrier =
            CD3DX12_RESOURCE_BARRIER::Transition(
                m_resource,
                m_currentState,
                _newState
            );

        Graphics::getGraphicsCommandList()->ResourceBarrier(1, &barrier);

        m_currentState = _newState;
    }

    Texture::~Texture() {
        DX12_OBJ_RELEASE(m_resource)
    }
    
    ID3D12Resource* Texture::getResource() const {
        return m_resource;
    }

    D3D12_GPU_VIRTUAL_ADDRESS Texture::getGPUAddress() const {
        return m_resource->GetGPUVirtualAddress();
    }

    DXGI_FORMAT Texture::getFormat() const {
        return m_desc.Format;
    }

    uint Texture::getWidth() const {
        return m_width;
    }

    uint Texture::getHeight() const {
        return m_height;
    }
}