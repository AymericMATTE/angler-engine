#include "pch.h"
#include "ray.h"
#include "application.h"

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

angler::Ray angler::Ray::raycastFromScreen(DirectX::XMFLOAT2 _screenCoord, float _length)
{
	angler::Camera3DComponent* cam = angler::Application::get().getActiveScene()->GetMainCamera();
	DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&cam->m_camera3D.getProj());
	proj = DirectX::XMMatrixInverse(nullptr, proj);
	DirectX::XMFLOAT3 start = cam->getOwner()->getPosition();
	DirectX::XMFLOAT3 NDC;
	DirectX::XMINT2 screen = { angler::Application::get().getSettings().window_width, angler::Application::get().getSettings().window_height };
	NDC.x = (2.0f * _screenCoord.x) / screen.x - 1.0f;
	NDC.y = 1.0f - (2.0f * _screenCoord.y) / screen.y;
	NDC.z = 1.0f;
	DirectX::XMFLOAT4 ndcFlat(NDC.x, NDC.y, -1.0f, 1.0f);
	DirectX::XMVECTOR eye = DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&ndcFlat), proj);
	eye = DirectX::XMVectorSet(eye.m128_f32[0], eye.m128_f32[1], -1.0f, 0.0f);
	DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&cam->m_camera3D.getView());

	DirectX::XMVECTOR rayWorld = DirectX::XMVector4Transform(eye, DirectX::XMMatrixInverse(nullptr, view));
	// don't forget to normalise the vector at some point
	DirectX::XMFLOAT3 rayWorldNormalized;
	DirectX::XMStoreFloat3(&rayWorldNormalized, DirectX::XMVector3Normalize(rayWorld));
	
	return angler::Ray(start, rayWorldNormalized, _length);
}
