#pragma once

// Local dependencies
#include "common/types.h"
#include "../pipeline-states/graphics-pso.h"
#include "../input-assembler/input-layout.h"
#include "../root-signatures/graphics-root-sig.h"

namespace angler {
	class Shader final {
	public:
		Shader() = default;

		void initialize(wcstr _vs, wcstr _ps, const GraphicsRootSig* _rootSig, const InputLayout* _inputLayout);
		
		void setAlpha(const bool _alphaEnabled);
		
		[[nodiscard]] bool isAlphaEnabled() const;
		[[nodiscard]] GraphicsPSO* getPSO();
		[[nodiscard]] GraphicsRootSig* getRootSignature();
		[[nodiscard]] InputLayout* getInputLayout();

		~Shader();
		
		IDxcResult* getVertexResult() const;
		IDxcResult* getPixelResult() const;

	private:
		IDxcResult* m_compiledVS = nullptr;
		IDxcResult* m_compiledPS = nullptr;
		
		GraphicsPSO m_opaque;
		GraphicsPSO m_alpha;
		GraphicsRootSig m_rootSignature;
		InputLayout m_inputLayout;
		bool m_isAlphaEnabled = false;
	};
}