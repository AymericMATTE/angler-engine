#include "pch.h"
#include "swap-chain.h"
#include "../graphics/graphics.h"

using Microsoft::WRL::ComPtr;

namespace angler {
	void SwapChain::initialize(HWND window, uint width, uint height) {
		ComPtr<IDXGIFactory4> factory = Graphics::getFactory();
		ID3D12CommandQueue* commandQueue = Graphics::getCommandQueue();
		
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width = width;
		desc.Height = height;
		desc.Format = Graphics::getRenderTargetFormat();
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = m_bufferCount;

		// I Disable MSAA (For now) Since Sylvain said we need to do it in a separate RT
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.Scaling = DXGI_SCALING_STRETCH;
		desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING; // I disabled vsync by default..

		ComPtr<IDXGISwapChain1> swapChain1; // I just copied an example using CompPtr, TO DO : Remove that shit..

		ThrowIfFailed(factory->CreateSwapChainForHwnd(
			commandQueue,
			window,
			&desc,
			nullptr,
			nullptr,
			&swapChain1));

		// Disable Alt+Enter (Since fullscreen seems to have a problem right-now..)
		ThrowIfFailed(factory->MakeWindowAssociation(window, DXGI_MWA_NO_ALT_ENTER));

		// Convert to SwapChain3
		ThrowIfFailed(swapChain1.As(&m_swapChain));

		m_current = m_swapChain->GetCurrentBackBufferIndex();

		// Create RT
		for (uint i = 0; i < m_bufferCount; ++i) {
			ComPtr<ID3D12Resource> resource;
			ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&resource)));

			m_swapChainBuffer[i].initialize(resource.Get(), width, height);
		}
	}

	void SwapChain::present(bool vsync) const {
		UINT syncInterval = vsync ? 1 : 0;
		UINT flags = 0;

		if (!vsync)
			flags = DXGI_PRESENT_ALLOW_TEARING;

		ThrowIfFailed(m_swapChain->Present(syncInterval, flags));
	}

	RenderTarget* SwapChain::getCurrentRenderTarget() {
		m_current = m_swapChain->GetCurrentBackBufferIndex();
		return &m_swapChainBuffer[m_current];
	}
	
	uint SwapChain::getCurrentBackBufferIndex() const {
		return m_current;
	}
}