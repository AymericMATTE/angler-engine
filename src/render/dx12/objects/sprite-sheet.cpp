#include "pch.h"
#include "sprite-sheet.h"
#include "render/dx12/graphics/graphics.h"
#include "render/renderer/material.h"
#include "render/dx12/objects/geometry.h"

namespace angler {
	SpriteSheet::SpriteSheet(wcstr _filePath)
	{
		texture.loadFromDDS(_filePath);
		Graphics::addTextureToHeap(&texture);

		count = 1;
		size = DirectX::XMUINT2(1, 1);
		spacing = DirectX::XMUINT2(0, 0);
		offset = DirectX::XMUINT2(0, 0);
		spriteSize = DirectX::XMUINT2(texture.getWidth(), texture.getHeight());
	}

	SpriteSheet::SpriteSheet(wcstr _filePath, uint _spriteCount, uint _spritesPerRow, DirectX::XMUINT2 _sheetOffset, DirectX::XMUINT2 _spriteSpacing)
	{
		assert(_spritesPerRow != 0);

		texture.loadFromDDS(_filePath);
		Graphics::addTextureToHeap(&texture);

		count = _spriteCount;

		size = DirectX::XMUINT2(_spritesPerRow, (count-1) / _spritesPerRow + 1);

		spacing = _spriteSpacing;
		offset = _sheetOffset;

		spriteSize = DirectX::XMUINT2((texture.getWidth() - _sheetOffset.x - _spriteSpacing.x * (_spritesPerRow - 1)) / _spritesPerRow, 
			(texture.getHeight() + _sheetOffset.y + _spriteSpacing.y * ((_spriteCount - 1) / _spritesPerRow)) / ((_spriteCount - 1) / _spritesPerRow + 1));
	}

	void SpriteSheet::Cut(uint _spriteCount, uint _spritesPerRow, DirectX::XMUINT2 _sheetOffset, DirectX::XMUINT2 _spriteSpacing)
	{
		count = _spriteCount;

		size = DirectX::XMUINT2(_spritesPerRow, (count - 1) / _spritesPerRow + 1);

		spacing = _spriteSpacing;
		offset = _sheetOffset;

		spriteSize = DirectX::XMUINT2((texture.getWidth() - _sheetOffset.x - _spriteSpacing.x * (_spritesPerRow - 1)) / _spritesPerRow,
			(texture.getHeight() + _sheetOffset.y + _spriteSpacing.y * ((_spriteCount - 1) / _spritesPerRow)) / ((_spriteCount - 1) / _spritesPerRow + 1));
	}

	DirectX::XMUINT4 SpriteSheet::GetSpriteRect(uint _index) {
		assert(_index <= count);

		uint left = (_index % size.x) * (spacing.x + spriteSize.x) + offset.x;
		uint top = (_index / size.x) * (spacing.y + spriteSize.y) + offset.y;
		uint right = ((_index % size.x) + 1) * spriteSize.x + offset.x;
		uint bottom = ((_index / size.x) + 1)* spriteSize.y + offset.y;

		return DirectX::XMUINT4(left, top, right, bottom);
	}

	void SpriteSheet::SetCurrentFrame(uint _frame) {
		currentFrame = _frame;
	}

	void SpriteSheet::NextFrame() {
		if (currentFrame + 1 < count) {
			currentFrame++;
			return;
		}

		if (loop) {
			currentFrame = 0;
			return;
		}
	}

	DirectX::XMFLOAT4X4 SpriteSheet::getTextureTransform()
	{
		DirectX::XMFLOAT2 maxFloat(static_cast<float>((spriteSize.x + spacing.x) * size.x + offset.x),
			static_cast<float>((spriteSize.y + spacing.y) * size.y + offset.y));
		DirectX::XMVECTOR max = DirectX::XMLoadFloat2(&maxFloat);

		DirectX::XMUINT4 spriteRect(GetSpriteRect(currentFrame));
		DirectX::XMFLOAT4 textureCoords;
		DirectX::XMStoreFloat4(&textureCoords, DirectX::XMVectorDivide(DirectX::XMLoadUInt4(&spriteRect), max));
		
		DirectX::XMFLOAT4X4 texTransform;
		DirectX::XMStoreFloat4x4(&texTransform, DirectX::XMMatrixTranspose(DirectX::XMMATRIX(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			textureCoords.x, textureCoords.y, 0, 1)));

		return texTransform;
	}

	Font::Font(wcstr _filePath, uint _spriteCount, uint _spritesPerRow, DirectX::XMUINT2 _sheetOffset, DirectX::XMUINT2 _spriteSpacing): SpriteSheet(_filePath, _spriteCount, _spritesPerRow, _sheetOffset, _spriteSpacing) {}
}
