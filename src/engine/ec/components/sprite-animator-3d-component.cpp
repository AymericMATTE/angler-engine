#include "pch.h"
#include "sprite-animator-3d-component.h"
#include "mesh-component.h"
#include "application.h"

namespace angler {
    SpriteSheet* SpriteAnimator3DComponent::getSprite() const {
        return m_sprite;
    }

    void SpriteAnimator3DComponent::setSprite(SpriteSheet* _sprite) {
        m_sprite = _sprite;

        MeshComponent* mesh = getOwner()->getComponent<MeshComponent>();

        if (m_material) delete m_material;

        m_material = new angler::Material(Application::get().getRenderer().getDefault3DShader());
        m_material->setProperty("albedoID", m_sprite->texture.getId());
        m_material->setProperty("baseColor", DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
        m_material->setProperty("metallic", 0.0f);
        m_material->setProperty("roughness", 1.0f);
        m_material->setProperty("specular", 0.5f);

        mesh->setMaterial(m_material);
        mesh->setUVScale(DirectX::XMFLOAT2(m_sprite->size.x, m_sprite->size.y));
    }

    void SpriteAnimator3DComponent::addAnimation(SpriteAnimation& _animation, std::string _name)
    {
        m_animations[_name] = _animation;
    }

    void SpriteAnimator3DComponent::play(std::string _name)
    {
        m_currentAnimation = &m_animations[_name];
    }

    void SpriteAnimator3DComponent::OnUpdate()
    {
        if (m_active == false) return;
        if (m_material == nullptr) return;
        if (getOwner()->hasComponent<MeshComponent>() == false) return;

        m_frameTimer += Application::get().deltaTime();

        if (m_frameTimer >= m_currentAnimation->frames[m_currentAnimation->currentFrame].frameTime) {
            m_frameTimer -= m_currentAnimation->frames[m_currentAnimation->currentFrame].frameTime;

            m_currentAnimation->nextFrame();
        }

        DirectX::XMINT2 coord = m_currentAnimation->frames[m_currentAnimation->currentFrame].texturePos;
        m_sprite->SetCurrentFrame(coord.x + m_sprite->size.x * coord.y);
        MeshComponent* mesh = getOwner()->getComponent<MeshComponent>();
        mesh->setUVOffset(m_sprite->getUVOffset());
    }

    
}

void SpriteAnimation::nextFrame()
{
    if (looping) {
        currentFrame = (currentFrame + 1) % frames.size();
    }
    else {
        currentFrame = currentFrame + 1 >= frames.size() ? frames.size() : currentFrame + 1;
    }
}
