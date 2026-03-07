#pragma once
#include <DirectXMath.h>
#include <vector>

struct Transform {
public:
    Transform();

    void setParent(Transform* _parent);

    void translate(const DirectX::XMFLOAT3& _delta);
    void rotate(const DirectX::XMFLOAT4& _deltaQuat);
    void rotate(const DirectX::XMFLOAT3& _deltaEuler);
    void scale(const DirectX::XMFLOAT3& _factor);

    void setPosition(const DirectX::XMFLOAT3& _pos);
    void setRotation(const DirectX::XMFLOAT4& _quat);
    void setRotation(const DirectX::XMFLOAT3& _euler);
    void setScale(const DirectX::XMFLOAT3& _scale);

    void localTranslate(const DirectX::XMFLOAT3& _delta);
    void localRotate(const DirectX::XMFLOAT4& _deltaQuat);
    void localRotate(const DirectX::XMFLOAT3& _deltaEuler);
    void localScale(const DirectX::XMFLOAT3& _factor);

    void setLocalPosition(const DirectX::XMFLOAT3& _pos);
    void setLocalRotation(const DirectX::XMFLOAT4& _quat);
    void setLocalRotation(const DirectX::XMFLOAT3& _euler);
    void setLocalScale(const DirectX::XMFLOAT3& _scale);

    void setWorldTransform(const DirectX::XMFLOAT3& _pos, const DirectX::XMFLOAT4& _quat, const DirectX::XMFLOAT3& _scale);

    [[nodiscard]] DirectX::XMFLOAT3 getLocalPosition() const { return m_localPosition; }
    [[nodiscard]] DirectX::XMFLOAT4 getLocalRotation() const { return m_localRotation; }
    [[nodiscard]] DirectX::XMFLOAT3 getLocalScale() const { return m_localScale; }

    [[nodiscard]] DirectX::XMFLOAT4X4 getWorldMatrix();
    [[nodiscard]] DirectX::XMFLOAT4X4 getTransposedWorldMatrix();
    [[nodiscard]] DirectX::XMFLOAT3 getPosition();
    [[nodiscard]] DirectX::XMFLOAT4 getRotation();
    [[nodiscard]] DirectX::XMFLOAT3 getScale();

    [[nodiscard]] DirectX::XMFLOAT3 getForward();
    [[nodiscard]] DirectX::XMFLOAT3 getRight();
    [[nodiscard]] DirectX::XMFLOAT3 getUp();

    void lookAt(const DirectX::XMFLOAT3& _target, const DirectX::XMFLOAT3& _up = { 0.f, 1.f, 0.f });
    void lookTo(const DirectX::XMFLOAT3& _direction, const DirectX::XMFLOAT3& _up = { 0.f, 1.f, 0.f });

private:
    void setDirty();
    void updateValues();

    Transform* m_parent = nullptr;

    DirectX::XMFLOAT3 m_localPosition = { 0.f, 0.f, 0.f };
    DirectX::XMFLOAT4 m_localRotation = { 0.f, 0.f, 0.f, 1.f };
    DirectX::XMFLOAT3 m_localScale = { 1.f, 1.f, 1.f };

    DirectX::XMFLOAT4X4 m_worldMatrix;
    DirectX::XMFLOAT3 m_worldPosition = { 0.f, 0.f, 0.f };
    DirectX::XMFLOAT4 m_worldRotation = { 0.f, 0.f, 0.f, 1.f };
    DirectX::XMFLOAT3 m_worldScale = { 1.f, 1.f, 1.f };

    bool m_isDirty = true;
};