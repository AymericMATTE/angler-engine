#pragma once
#include <DirectXMath.h>
#include <vector>

struct Transform {
public:
    Transform();
    ~Transform();

    void setParent(Transform* newParent);
    void addChild(Transform* newChild);
    void removeChild(Transform* _child);

    void translate(const DirectX::XMFLOAT3& delta);
    void rotate(const DirectX::XMFLOAT4& deltaQuat);
    void rotate(const DirectX::XMFLOAT3& deltaEuler);
    void scale(const DirectX::XMFLOAT3& factor);

    void setPosition(const DirectX::XMFLOAT3& pos);
    void setRotation(const DirectX::XMFLOAT4& quat);
    void setRotation(const DirectX::XMFLOAT3& euler);
    void setScale(const DirectX::XMFLOAT3& scale);

    void localTranslate(const DirectX::XMFLOAT3& delta);
    void localRotate(const DirectX::XMFLOAT4& deltaQuat);
    void localRotate(const DirectX::XMFLOAT3& deltaEuler);
    void localScale(const DirectX::XMFLOAT3& factor);

    void setLocalPosition(const DirectX::XMFLOAT3& pos);
    void setLocalRotation(const DirectX::XMFLOAT4& quat);
    void setLocalRotation(const DirectX::XMFLOAT3& euler);
    void setLocalScale(const DirectX::XMFLOAT3& scale);

    void setWorldTransform(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT4& quat, const DirectX::XMFLOAT3& scale);

    DirectX::XMFLOAT3 getLocalPosition() const { return m_localPosition; }
    DirectX::XMFLOAT4 getLocalRotation() const { return m_localRotation; }
    DirectX::XMFLOAT3 getLocalScale() const { return m_localScale; }

    DirectX::XMFLOAT4X4 getWorldMatrix();
    DirectX::XMFLOAT4X4 getTransposedWorldMatrix();
    DirectX::XMFLOAT3 getPosition();
    DirectX::XMFLOAT4 getRotation();
    DirectX::XMFLOAT3 getScale();

    DirectX::XMFLOAT3 getForward();
    DirectX::XMFLOAT3 getRight();
    DirectX::XMFLOAT3 getUp();

    void lookAt(const DirectX::XMFLOAT3& _target, const DirectX::XMFLOAT3& _up = { 0.f, 1.f, 0.f });
    void lookTo(const DirectX::XMFLOAT3& _direction, const DirectX::XMFLOAT3& _up = { 0.f, 1.f, 0.f });

private:
    void setDirty();
    void updateValues();

    Transform* m_parent = nullptr;
    std::vector<Transform*> m_childrens = {};

    DirectX::XMFLOAT3 m_localPosition = { 0.f, 0.f, 0.f };
    DirectX::XMFLOAT4 m_localRotation = { 0.f, 0.f, 0.f, 1.f };
    DirectX::XMFLOAT3 m_localScale = { 1.f, 1.f, 1.f };

    DirectX::XMFLOAT4X4 m_worldMatrix;
    DirectX::XMFLOAT3 m_worldPosition = { 0.f, 0.f, 0.f };
    DirectX::XMFLOAT4 m_worldRotation = { 0.f, 0.f, 0.f, 1.f };
    DirectX::XMFLOAT3 m_worldScale = { 1.f, 1.f, 1.f };

    bool m_isDirty = true;
};