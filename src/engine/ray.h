#pragma once
#include <DirectXMath.h>
#include "common/physics.h"

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

		//static Ray& raycastFromScreen(DirectX::XMFLOAT2 _screenCoord, float _length = 100);
		static void raycastFromScreen(Ray* _out, DirectX::XMFLOAT2 _screenCoord, float _length = 100);
	};

	

}
