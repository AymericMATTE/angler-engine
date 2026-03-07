#pragma once

#include "common/types.h"
#include "render-target.h"

#include <dxgi1_6.h>
#include <wrl.h>

namespace angler {

    class SwapChain final {
    public:
        SwapChain(const SwapChain&) = delete;
        SwapChain(SwapChain&&) noexcept = delete;
        SwapChain& operator=(const SwapChain&) = delete;
        SwapChain& operator=(SwapChain&&) noexcept = delete;

        SwapChain() = default;
        ~SwapChain() = default;

        void initialize(HWND window, uint width, uint height);
        void present(bool vsync) const;

        RenderTarget* getCurrentRenderTarget();
        uint getCurrentBackBufferIndex() const;

    private:
        static constexpr uint m_bufferCount = 2;

        Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
        RenderTarget m_swapChainBuffer[m_bufferCount];

        uint m_current = 0;
        bool m_allowTearing = false;
    };

}