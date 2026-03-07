#pragma once

// Local dependencies
#include "../pso.h"
#include "../utils/d3dx12.h"

// External dependencies
#include <dxcapi.h>

// Forward declarations
namespace angler {
	class GraphicsRootSig;
	class InputLayout;
}

namespace angler {
	class GraphicsPSO : public PSO {		
	public:
		void initialize(const GraphicsRootSig* _rootSignature, const InputLayout* _inputLayout,
			IDxcBlob* _vs, IDxcBlob* _ps,
			DXGI_FORMAT _backBufferFormat, DXGI_FORMAT _depthStencilFormat, 
			bool _blend = false, int _4xMsaaQuality = -1);
	};
}