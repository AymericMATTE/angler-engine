#pragma once

// Local dependencies
#include "utils/d3dx12.h"
#include "common/types.h"

// External dependencies
#include <vector>

namespace angler {
	class RootSig {
	protected:
		ID3D12RootSignature* m_rootSignature = nullptr;
		std::vector<CD3DX12_ROOT_PARAMETER> m_parameters = {};

	public:
		RootSig() = default;
		RootSig(const RootSig& _other);
		RootSig(RootSig&& _other) noexcept;
		RootSig& operator=(const RootSig& _other);
		RootSig& operator=(RootSig&& _other) noexcept;

		virtual void initialize();

		[[nodiscard]] ID3D12RootSignature* get() const;
		[[nodiscard]] uint getParameterCount() const;
		[[nodiscard]] std::vector<CD3DX12_ROOT_PARAMETER>& getParameters();

		virtual ~RootSig();
	};
}