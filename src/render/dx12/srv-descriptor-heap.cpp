#include "pch.h"
#include "srv-descriptor-heap.h"

// Local dependencies
#include "graphics/graphics.h"
#include "objects/texture.h"
#include "render/dx12/surfaces/render-target.h"

// External dependencies
#include <cassert>

namespace angler {

	void SRVDescriptorHeap::init(uint _capacity)
	{
		DX12_OBJ_RELEASE(m_heap);
		m_unusedIdPool = {};
		m_size = 0;
		m_capacity = _capacity;

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
		heapDesc.NumDescriptors = _capacity;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heapDesc.NodeMask = 0;
		ThrowIfFailed(Graphics::getDevice()->CreateDescriptorHeap(
			&heapDesc, IID_PPV_ARGS(&m_heap)))
	}

	void SRVDescriptorHeap::addShaderResourceView(Texture* _resource)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = _resource->getSRVDesc();

		_resource->m_id = getNextId();
		_resource->m_table = this;

		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = getCPUHandle(_resource->m_id);

		Graphics::getDevice()->CreateShaderResourceView(_resource->getResource(), &desc, handle);
	}

	void SRVDescriptorHeap::addShaderResourceView(RenderTarget* _resource)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = _resource->getSRVDesc();

		_resource->m_id = getNextId();
		_resource->m_table = this;

		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = getCPUHandle(_resource->m_id);

		Graphics::getDevice()->CreateShaderResourceView(_resource->getResource(), &desc, handle);
	}

	D3D12_DESCRIPTOR_RANGE SRVDescriptorHeap::getDescriptorRange() const
	{
		D3D12_DESCRIPTOR_RANGE range = {};
		range.NumDescriptors = m_capacity;
		range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		return range;
	}

	void SRVDescriptorHeap::remove(uint _id)
	{
		m_unusedIdPool.push(_id);
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE SRVDescriptorHeap::getCPUHandle(uint _offset) const
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_heap->GetCPUDescriptorHandleForHeapStart();
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(handle, _offset, Graphics::getCbvSrvUavDescriptorSize());
	}

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRVDescriptorHeap::getGPUHandle(uint _offset) const
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handle = m_heap->GetGPUDescriptorHandleForHeapStart();
		return CD3DX12_GPU_DESCRIPTOR_HANDLE(handle, _offset, Graphics::getCbvSrvUavDescriptorSize());
	}

	ID3D12DescriptorHeap* SRVDescriptorHeap::getHeap() const
	{
		return m_heap;
	}

	SRVDescriptorHeap::~SRVDescriptorHeap()
	{
		DX12_OBJ_RELEASE(m_heap);
	}

	uint SRVDescriptorHeap::getNextId()
	{
		if (m_unusedIdPool.empty()) {
			assert(m_size < m_capacity);
			return m_size++;
		}
		else {
			uint unusedIndex = m_unusedIdPool.front();
			m_unusedIdPool.pop();
			return unusedIndex;
		}
	}
}