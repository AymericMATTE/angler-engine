#pragma once

// Local dependencies
#include "common/types.h"
#include "../root-sig.h"

namespace angler {
	class GraphicsRootSig : public RootSig {
	public:
		GraphicsRootSig(const GraphicsRootSig& _other) = default;
		GraphicsRootSig(GraphicsRootSig&& _other) noexcept = default;
		GraphicsRootSig& operator=(const GraphicsRootSig& _other) = default;
		GraphicsRootSig& operator=(GraphicsRootSig&& _other) noexcept = default;
		
		explicit GraphicsRootSig();

		void initialize() override;

		~GraphicsRootSig() override = default;

		void addConstantBufferView(uint _shaderRegister);
		void addUnorderedAccessView(uint _shaderRegister);
		void addShaderResourceView(uint _shaderRegister);

		[[nodiscard]] uint getFixedParameterCount() const;
	};
}