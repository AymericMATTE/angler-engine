#pragma once

// External dependencies
#include <DirectXMath.h>

namespace angler {

	// All of this code is inspired by the frustum culling from : https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling
	struct Plane
	{
		// unit vector
		DirectX::XMFLOAT3 normal = { 0.f, 1.f, 0.f };

		// distance from origin to the nearest point in the plane
		float distance = 0.f;

		Plane() = default;

		Plane(const DirectX::XMFLOAT3& _normal, const float _distance) : normal(_normal), distance(_distance) {}

		float getSignedDistanceToPlane(const DirectX::XMFLOAT3& _point) const;

		void _vectorcall CreatePlaneFromVector(DirectX::FXMVECTOR v);
	};

	struct Frustum
	{
		Plane topFace;
		Plane bottomFace;

		Plane rightFace;
		Plane leftFace;

		Plane farFace;
		Plane nearFace;
	};
}