#pragma once

#include "render/dx12/objects/sprite-sheet.h"

#include <DirectXMath.h>
#include <map>
#include "engine/ec/component-base.h"

struct SpriteAnimationFrame {
    DirectX::XMINT2 texturePos;
    float frameTime;
};

struct SpriteAnimation {
    std::vector<SpriteAnimationFrame> frames;
    int currentFrame = 0;

    void nextFrame();

    bool looping = true;
};

namespace angler {
    struct SpriteAnimator3DComponent : public ComponentBase<SpriteAnimator3DComponent> {
        SpriteSheet* getSprite() const;

        void setSprite(SpriteSheet* _sprite);
        void addAnimation(SpriteAnimation& _animation, std::string _name);
        void play(std::string _name);

        void OnUpdate();
        
        void start() { m_active = true; };
        void pause() { m_active = false; };

    private:
        bool m_active = true;

        std::unordered_map<std::string, SpriteAnimation> m_animations;
        SpriteAnimation* m_currentAnimation;

        float m_frameTimer = 0;

        Material* m_material;
        SpriteSheet* m_sprite = nullptr;
    };
}