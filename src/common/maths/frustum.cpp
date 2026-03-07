#include "pch.h"
#include "frustum.h"

namespace angler {

	using namespace DirectX;

	float Plane::getSignedDistanceToPlane(const DirectX::XMFLOAT3& point) const {
		XMVECTOR result = XMVector3Dot(XMLoadFloat3(&normal), XMLoadFloat3(&point));
		result = XMVectorAdd(result, XMVectorReplicate(distance));
		return XMVectorGetX(result);
	}

	void Plane::CreatePlaneFromVector(DirectX::FXMVECTOR v) {
		XMFLOAT4 p;
		XMStoreFloat4(&p, XMPlaneNormalize(v));

		normal = XMFLOAT3(p.x, p.y, p.z);
		distance = p.w;
	}
}