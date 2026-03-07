#include "pch.h"
#include "graphics-pso.h"

// Local dependencies
#include "../graphics/graphics.h"
#include "../input-assembler/input-layout.h"
#include "../root-signatures/graphics-root-sig.h"

namespace angler  {
	void GraphicsPSO::initialize(const GraphicsRootSig* _rootSignature, const InputLayout* _inputLayout, IDxcBlob* _vs, IDxcBlob* _ps,
		DXGI_FORMAT _backBufferFormat, DXGI_FORMAT _depthStencilFormat, bool _alpha, int _4xMsaaQuality) {
		
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { _inputLayout->get()->data(), (uint)_inputLayout->get()->size()};
		psoDesc.VS = {
			 static_cast<BYTE*>(_vs->GetBufferPointer()),
			 _vs->GetBufferSize()
		};
		psoDesc.PS = {
			 static_cast<BYTE*>(_ps->GetBufferPointer()),
			 _ps->GetBufferSize()
		};
		
		psoDesc.pRootSignature = _rootSignature->get();

		D3D12_BLEND_DESC blendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		if (_alpha) {
			D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc;
			transparencyBlendDesc.BlendEnable = true;
			transparencyBlendDesc.LogicOpEnable = false;
			transparencyBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			transparencyBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			transparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
			transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
			transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
			transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
			transparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
			transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

			blendState.RenderTarget[0] = transparencyBlendDesc;
		}

		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = blendState;
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;

		bool msaaState = _4xMsaaQuality != -1;
		psoDesc.SampleDesc.Count = msaaState ? 4 : 1;
		psoDesc.SampleDesc.Quality = msaaState ? (_4xMsaaQuality - 1) : 0;

		psoDesc.RTVFormats[0] = _backBufferFormat;
		psoDesc.DSVFormat = _depthStencilFormat;
		
		ThrowIfFailed(Graphics::getDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pso)))
	}
}