#include "pch.h"
#include "maths.h"

namespace angler {
    DirectX::XMFLOAT4 eulerToQuat(float _pitch, float _yaw, float _roll) {
        // Convert degrees to radians
        float pitchRad = DirectX::XMConvertToRadians(_pitch);
        float yawRad   = DirectX::XMConvertToRadians(_yaw);
        float rollRad  = DirectX::XMConvertToRadians(_roll);

        // Create rotation quaternions for each axis
        DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYaw(pitchRad, yawRad, rollRad);
    
        // Normalize to ensure unit quaternion
        quat = DirectX::XMQuaternionNormalize(quat);
    
        // Store result
        DirectX::XMFLOAT4 result;
        DirectX::XMStoreFloat4(&result, quat);
    
        return result;
    }

    DirectX::XMFLOAT4 eulerToQuatYPR(float _yaw, float _pitch, float _roll) {
        // Convert degrees to radians
        float yawRad   = DirectX::XMConvertToRadians(_yaw);
        float pitchRad = DirectX::XMConvertToRadians(_pitch);
        float rollRad  = DirectX::XMConvertToRadians(_roll);

        // Create individual axis rotations
        DirectX::XMVECTOR qYaw   = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0, 1, 0, 0), yawRad);
        DirectX::XMVECTOR qPitch = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(1, 0, 0, 0), pitchRad);
        DirectX::XMVECTOR qRoll  = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0, 0, 1, 0), rollRad);

        // Multiplication in reverse order (Roll * Pitch * Yaw)
        DirectX::XMVECTOR quat = DirectX::XMQuaternionMultiply(qRoll, DirectX::XMQuaternionMultiply(qPitch, qYaw));
        quat = DirectX::XMQuaternionNormalize(quat);
    
        // Store result
        DirectX::XMFLOAT4 result;
        DirectX::XMStoreFloat4(&result, quat);
    
        return result;
    }
}