#include "pch.h"
#include "camera-3d-component.h"
#include "engine/ec/game-object.h"

namespace angler {
    Camera3DComponent::Camera3DComponent() : m_camera3D(65.0f) {
        m_isMain = true;
    }

    void Camera3DComponent::OnPreRender()
    {
        m_camera3D.setWorldMatrix(getOwner()->getWorldMatrix());
    }
    
    float Camera3DComponent::getFov() const {
        return m_camera3D.getFov();
    }
    
    void Camera3DComponent::setFov(float _fov) const {
        m_camera3D.setFov(_fov);
    }
    
    float Camera3DComponent::getNear() const {
        return m_camera3D.getNear();
    }
    
    void Camera3DComponent::setNear(float _near) const {
        m_camera3D.setNear(_near);
    }
    
    float Camera3DComponent::getFar() const {
        return m_camera3D.getFar();
    }
    
    void Camera3DComponent::setFar(float _far) const {
        m_camera3D.setFar(_far);
    }

    bool Camera3DComponent::isMain() const {
        return m_isMain;
    }
}