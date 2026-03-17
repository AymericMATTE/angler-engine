#include "pch.h"
#include "ray.h"

angler::Ray::Ray(DirectX::XMFLOAT3 _start, DirectX::XMFLOAT3 _direction, float _length)
{
	start = _start;
	direction = _direction;
	length = _length;
	bounds->type = GameBounds::BOX;
}

angler::Ray::~Ray()
{
	delete bounds;
}

void angler::Ray::updateBounds()
{
	bounds->box->extents.xmF = { 0.1f, 0.1f, length/2.f };
}
