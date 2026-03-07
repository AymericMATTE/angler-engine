#pragma once

// Local dependencies
#include "common/types.h"
#include "../root-sig.h"

namespace angler {
	class ComputeRootSig : public RootSig {
	public:
		ComputeRootSig() = default;
		ComputeRootSig(const ComputeRootSig& _other) = default;
		ComputeRootSig(ComputeRootSig&& _other) noexcept = default;
		ComputeRootSig& operator=(const ComputeRootSig& _other) = default;
		ComputeRootSig& operator=(ComputeRootSig&& _other) noexcept = default;

		~ComputeRootSig() override = default;

		void addConstantBufferView(uint _shaderRegister);
		void addUnorderedAccessView(uint _shaderRegister);
		void addShaderResourceView(uint _shaderRegister);
	};
}