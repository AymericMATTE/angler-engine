#pragma once

#include <DirectXMath.h>

struct Transform {
public:
    void setParent(Transform* _parent);
    [[nodiscard]] Transform* getParent() const;

    void setPosition(const DirectX::XMFLOAT3& _position);
    void setRotation(const DirectX::XMFLOAT4& _rotation);
    void setRotation(const DirectX::XMFLOAT3& _rotation);
    void setScale(const DirectX::XMFLOAT3& _scale);

    void translate(const DirectX::XMFLOAT3& _delta);
    void rotate(const DirectX::XMFLOAT4& _delta);
    void rotate(const DirectX::XMFLOAT3& _delta);
    void scale(const DirectX::XMFLOAT3& _factor);

    void setLocalPosition(const DirectX::XMFLOAT3& _position);
    void setLocalRotation(const DirectX::XMFLOAT4& _rotation);
    void setLocalScale(const DirectX::XMFLOAT3& _scale);

    void localTranslate(const DirectX::XMFLOAT3& _delta);
    void localRotate(const DirectX::XMFLOAT4& _delta);
    void localRotate(const DirectX::XMFLOAT3& _delta);
    void localScale(const DirectX::XMFLOAT3& _factor);

    void lookAt(const DirectX::XMFLOAT3& _target, const DirectX::XMFLOAT3& up = { 0.f, 1.f, 0.f });
    void lookTo(const DirectX::XMFLOAT3& _direction, const DirectX::XMFLOAT3& up = { 0.f, 1.f, 0.f });

    [[nodiscard]] DirectX::XMFLOAT3 getPosition();
    [[nodiscard]] DirectX::XMFLOAT4 getRotation();
    [[nodiscard]] DirectX::XMFLOAT3 getScale();

    [[nodiscard]] DirectX::XMFLOAT3 getLocalPosition() const;
    [[nodiscard]] DirectX::XMFLOAT4 getLocalRotation() const;
    [[nodiscard]] DirectX::XMFLOAT3 getLocalScale() const;

    [[nodiscard]] DirectX::XMFLOAT4X4 getLocalMatrix();
    [[nodiscard]] DirectX::XMFLOAT4X4 getWorldMatrix();
    [[nodiscard]] DirectX::XMFLOAT4X4 getTransposedWorldMatrix();

    [[nodiscard]] DirectX::XMFLOAT3 getRight();
    [[nodiscard]] DirectX::XMFLOAT3 getUp();
    [[nodiscard]] DirectX::XMFLOAT3 getForward();

private:
    void markDirty();
    void updateLocalMatrix();
    void updateWorldMatrix();

private:
    Transform* m_parent = nullptr;

    DirectX::XMFLOAT3 m_localPosition = { 0.f, 0.f, 0.f };
    DirectX::XMFLOAT4 m_localRotation = { 0.f, 0.f, 0.f, 1.f };
    DirectX::XMFLOAT3 m_localScale = { 1.f, 1.f, 1.f };

    DirectX::XMFLOAT3 m_worldPosition = { 0.f, 0.f, 0.f };
    DirectX::XMFLOAT4 m_worldRotation = { 0.f, 0.f, 0.f, 1.f };
    DirectX::XMFLOAT3 m_worldScale = { 1.f, 1.f, 1.f };

    DirectX::XMFLOAT4X4 m_localMatrix;
    DirectX::XMFLOAT4X4 m_worldMatrix;

    bool m_localDirty = true;
    bool m_worldDirty = true;
};