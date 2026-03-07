#pragma once

// Local dependencies
#include "utils/d3dx12.h"
#include "common/types.h"

// External dependencies
#include <queue>

namespace angler
{
	class Texture;
	class RenderTarget;
	class IGraphicsBuffer;
}

namespace angler {
	class SRVDescriptorHeap {
	public:
		SRVDescriptorHeap() = default;

		SRVDescriptorHeap(const SRVDescriptorHeap& _other) = delete;
		SRVDescriptorHeap(SRVDescriptorHeap&& _other) noexcept = delete;
		SRVDescriptorHeap& operator=(const SRVDescriptorHeap& _other) = delete;
		SRVDescriptorHeap& operator=(SRVDescriptorHeap&& _other) noexcept = delete;

		void init(uint _capacity);

		void addShaderResourceView(Texture* _resource);
		void addShaderResourceView(RenderTarget* _resource);

		D3D12_DESCRIPTOR_RANGE getDescriptorRange() const;

		void remove(uint _id);

		[[nodiscard]] CD3DX12_CPU_DESCRIPTOR_HANDLE getCPUHandle(uint _offset = 0) const;
		[[nodiscard]] CD3DX12_GPU_DESCRIPTOR_HANDLE getGPUHandle(uint _offset = 0) const;
		[[nodiscard]] ID3D12DescriptorHeap* getHeap() const;

		virtual ~SRVDescriptorHeap();

	private :
		uint getNextId();

		uint m_size = 0;
		uint m_capacity = 0;
		std::queue<uint> m_unusedIdPool = {};

		ID3D12DescriptorHeap* m_heap = nullptr;
	};
}