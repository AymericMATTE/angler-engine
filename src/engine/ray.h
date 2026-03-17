#pragma once
#include <DirectXMath.h>
#include "physics.h"
namespace angler{
	struct Ray
	{
		DirectX::XMFLOAT3 start;
		DirectX::XMFLOAT3 direction;
		float length = 100;

		GameBounds* bounds;

		Ray(DirectX::XMFLOAT3 _start, DirectX::XMFLOAT3 _direction, float _length);
		~Ray();

		void updateBounds();
	};

}
