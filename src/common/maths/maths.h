#pragma once

#include <DirectXMath.h>
#include <DirectXPackedVector.h>

namespace angler {
    /// <summary>
    /// Converts a XYZ rotation (in deg°) to a quaternion.
    /// </summary>
    /// <param name="_pitch">Rotation angle around the X axis (in deg°).</param>
    /// <param name="_yaw">Rotation angle around the Y axis (in deg°).</param>
    /// <param name="_roll">Rotation angle around the Z axis (in deg°).</param>
    /// <returns>Quaternion representing the given input rotation.</returns>
    DirectX::XMFLOAT4 eulerToQuat(float _pitch, float _yaw, float _roll);
    
    /// <summary>
    /// Same as 'eulerToQuat', but applies rotation sequentially in YPR order.
    /// </summary>
    /// <param name="_pitch">Rotation angle around the X axis (in deg°).</param>
    /// <param name="_yaw">Rotation angle around the Y axis (in deg°).</param>
    /// <param name="_roll">Rotation angle around the Z axis (in deg°).</param>
    /// <returns>Quaternion representing the given input rotation.</returns>
    DirectX::XMFLOAT4 eulerToQuatYPR(float _yaw, float _pitch, float _roll);
}