#include "pch.h"
#include "ray.h"
#include "application.h"

angler::Ray::Ray(DirectX::XMFLOAT3 _start, DirectX::XMFLOAT3 _direction, float _length)
{
	start = _start;
	direction = _direction;
	length = _length;

	DirectX::XMFLOAT3 center = { _start.x + direction.x * length / 2, _start.y + direction.y * length / 2, _start.z + direction.z * length / 2 };
	//center = { start.x, start.y, start.z };

	DirectX::XMFLOAT3 wordlUp(0.f, 1.f, 0.f);
	DirectX::XMMATRIX view = DirectX::XMMatrixLookToLH(DirectX::XMVectorZero(), DirectX::XMVector3Normalize(XMLoadFloat3(&_direction)), DirectX::XMLoadFloat3(&wordlUp));
	DirectX::XMFLOAT4X4 viewF;
	DirectX::XMStoreFloat4x4(&viewF, view);

	DirectX::XMFLOAT3 axes[3] = {
		{ viewF._11, viewF._21, viewF._31},
		{ viewF._12, viewF._22, viewF._32 },
		{ viewF._13, viewF._23, viewF._33 }
	};
	
	bounds = new GameBounds(OBB(center, axes, { 0.01f, 0.01f, _length / 2 }));
	bounds->sphere = new SphereBounds(Physics::sphereFromOBB(bounds->box));
}

angler::Ray::~Ray()
{
	delete bounds;
}

void angler::Ray::updateBounds()
{
	if (direction.x == 0 || direction.y == 0 || direction.z == 0) {
		return;
	}

	bounds->box->extents.xmF = { 0.01f, 0.01f, length/2.f };

	DirectX::XMFLOAT3 center = { start.x + direction.x * length / 2, start.y + direction.y * length / 2, start.z + direction.z * length / 2 };
	//center = { start.x, start.y, start.z };

	DirectX::XMFLOAT3 worldUp(0.f, 1.f, 0.f);
	DirectX::XMMATRIX view = DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&center), DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&direction)), DirectX::XMLoadFloat3(&worldUp));
	DirectX::XMFLOAT4X4 viewF;
	DirectX::XMStoreFloat4x4(&viewF, view);

	bounds->box->center.xmF = center;
	bounds->box->axes[0] = Vector3(DirectX::XMFLOAT3{ viewF._11, viewF._21, viewF._31 });
	bounds->box->axes[1] = Vector3(DirectX::XMFLOAT3{ viewF._12, viewF._22, viewF._32 });
	bounds->box->axes[2] = Vector3(DirectX::XMFLOAT3{ viewF._13, viewF._23, viewF._33 });
	bounds->box->extents.xmF = { 0.001f, 0.001f, length / 2 };
	*bounds->sphere = Physics::sphereFromOBB(bounds->box);
}

void angler::Ray::raycastFromScreen(Ray* _out, DirectX::XMFLOAT2 _screenCoord, float _length)
{
	angler::Camera3DComponent* cam = angler::Application::get().getActiveScene()->GetMainCamera();
	DirectX::XMFLOAT4X4 viewF = cam->m_camera3D.getView();
	DirectX::XMFLOAT4X4 projF = cam->m_camera3D.getProj();
	DirectX::XMMATRIX VPI = DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&viewF), DirectX::XMLoadFloat4x4(&projF));

	//VPI = DirectX::XMMatrixInverse(nullptr, VPI);
	DirectX::XMFLOAT3 _start = cam->getOwner()->getPosition();
	DirectX::XMFLOAT3 NDC;
	DirectX::XMINT2 screen = { angler::Application::get().getSettings().window_width, angler::Application::get().getSettings().window_height };
	NDC.x = (2.0f * _screenCoord.x) / screen.x - 1.0f;
	NDC.y = 1.0f - (2.0f * _screenCoord.y) / screen.y;
	NDC.z = 1.0f;
	DirectX::XMFLOAT4 clipPoint(NDC.x, NDC.y, 1.0f, 1.0f);
	DirectX::XMVECTOR rayWorld = DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&clipPoint), VPI);

	DirectX::XMFLOAT3 rayWorldNormalized;
	DirectX::XMStoreFloat3(&rayWorldNormalized, DirectX::XMVector3Normalize(rayWorld));
	
	_out->start = _start;
	_out->direction = rayWorldNormalized;
	_out->length = _length;
}
