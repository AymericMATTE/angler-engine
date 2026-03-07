#pragma once
#include <DirectXMath.h>
#include "common/types.h"
#include "render/dx12/objects/texture.h"
#include "render/dx12/objects/mesh.h"
#include "render/renderer/material.h"

namespace angler{
	struct SpriteSheet {
		Texture texture;

		// Amount of sprites
		uint count;
		uint currentFrame = 0;
		bool loop = true;

		// Size of a single sprite
		DirectX::XMUINT2 spriteSize;

		// Size of the sprite sheet
		DirectX::XMUINT2 size;

		// Offset of first sprite from top-left
		DirectX::XMUINT2 offset;

		// Spacing between all sprites
		DirectX::XMUINT2 spacing;

		Mesh* mesh;
		Material* mat;

		SpriteSheet(wcstr _filePath);
		SpriteSheet(wcstr _filePath, uint _spriteCount, uint _spritesPerRow, DirectX::XMUINT2 _sheetOffset = DirectX::XMUINT2(0, 0), DirectX::XMUINT2 _spriteSpacing = DirectX::XMUINT2(0, 0));

		void Cut(uint _spriteCount, uint _spritesPerRow, DirectX::XMUINT2 _sheetOffset = DirectX::XMUINT2(0, 0), DirectX::XMUINT2 _spriteSpacing = DirectX::XMUINT2(0, 0));

		DirectX::XMUINT4 GetSpriteRect(uint _index);
		void SetCurrentFrame(uint _frame);
		void NextFrame();

	private:
		DirectX::XMFLOAT4X4 getTextureTransform();

		friend class Renderer;
	};

	struct Font : SpriteSheet {
		Font(wcstr _filePath, 
			uint _spriteCount = 126 - 32, 
			uint _spritesPerRow = 10,
			DirectX::XMUINT2 _sheetOffset = DirectX::XMUINT2(0, 0),
			DirectX::XMUINT2 _spriteSpacing = DirectX::XMUINT2(0, 0));

		float scale = 1.0f;
	};
}
