#pragma once

#include <unordered_map>

#include "render/dx12/objects/static-mesh.h"
#include "render/dx12/objects/texture.h"

namespace angler {
    struct Font;
    struct SpriteSheet;
    class Renderer;
    
    class ResourceManager {
    public:
        static void init(Renderer* _renderer);
        
        static StaticMesh* getMesh(const std::string& _name);
        static Texture* getTexture(const std::string& _name);
        static SpriteSheet* getSprite(const std::string& _name);
        static SpriteSheet* getSprite(const std::string& _name, uint _spriteCount, uint _spritesPerRow);
        
        static Shader* getDefault3DShader();
        static Shader* getDefault2DShader();

    private:
        static ResourceManager* m_instance;

        Renderer* m_renderer;
        
        ResourceManager();
        ~ResourceManager() = default;
        
        std::unordered_map<std::string, StaticMesh*> m_meshes;
        std::unordered_map<std::string, Texture*> m_textures;
        std::unordered_map<std::string, Font*> m_fonts = {};
        std::unordered_map<std::string, SpriteSheet*> m_sprites = {};
        
        void loadMeshes();
        void loadMesh(const std::string& _name);
        void addMesh(const std::string& _name, const std::string& _path);
        
        void loadTextures();
        void loadTexture(const std::string& _name);
        void addTexture(const std::string& _name, const std::string& _path);

        void loadSprites();
        void loadSprite(const std::string& _name, uint _spriteCount, uint _spritesPerRow);
        void addSprite(const std::string& _name, const std::string& _path, uint _spriteCount, uint _spritesPerRow);
    };
}