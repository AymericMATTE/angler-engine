#include "pch.h"
#include "resource-manager.h"

#include "common.h"
#include "obj-importer.h"
#include "utils/os.h"
#include "render/dx12/objects/geometry.h"
#include "render/renderer/renderer.h"

namespace angler {
    ResourceManager* ResourceManager::m_instance = nullptr;
    
    void ResourceManager::init(Renderer* _renderer) {
        if (m_instance != nullptr) {
            std::cout << "WARNING : You cannot initialize the ResourceManager two times ! \n";
            return;
        }

        m_instance = new ResourceManager();
        m_instance->m_renderer = _renderer;
    }
    
    ResourceManager::ResourceManager() {
        //loadMeshes();
        //loadTextures();
    }
    
    void ResourceManager::loadMeshes() {
        std::vector<std::string> paths = os::getFilesInDirectory(RES_DIR, ".obj");
        
        for (auto& path : paths) {
            addMesh(os::getFileName(path), path);
        }
    }
    void ResourceManager::loadTextures() {
        std::vector<std::string> paths = os::getFilesInDirectory(RES_DIR, ".dds");
        
        for (auto& path : paths) {
            addTexture(os::getFileName(path), path);
        }
    }
    
    
    void ResourceManager::loadMesh(const std::string& _name) {
        addMesh(_name, os::getFileInDirectory(RES_DIR, _name + ".obj"));
    }
    void ResourceManager::loadTexture(const std::string& _name) {
        addTexture(_name, os::getFileInDirectory(RES_DIR, _name + ".dds"));
    }
    
    
    StaticMesh* ResourceManager::getMesh(const std::string& _name) {
        if (m_instance == nullptr) { return nullptr; }
        
        bool success = m_instance->m_meshes.contains(_name);

        if (!success) {
            m_instance->loadMesh(_name);
            success = m_instance->m_meshes.contains(_name);
            // TO DO : throw an error if could not load the model (or return nullptr idk)
        }
        return m_instance->m_meshes[_name];
    }
    Texture* ResourceManager::getTexture(const std::string& _name) {
        if (m_instance == nullptr) { return nullptr; }
        
        bool success = m_instance->m_textures.contains(_name);

        if (!success) {
            m_instance->loadTexture(_name);
            success = m_instance->m_textures.contains(_name);
            // TO DO : throw an error if could not load the model (or return nullptr idk)
        }
        return m_instance->m_textures[_name];
    }
    SpriteSheet* ResourceManager::getSprite(const std::string& _name) {
        if (m_instance == nullptr) { return nullptr; }
        
        bool success = m_instance->m_sprites.contains(_name);

        if (!success) {
            return nullptr;
        }
        return m_instance->m_sprites[_name];
    }

    SpriteSheet* ResourceManager::getSprite(const std::string& _name, uint _spriteCount, uint _spritesPerRow)
    {
        if (m_instance == nullptr) { return nullptr; }

        bool success = m_instance->m_sprites.contains(_name);

        if (!success) {
            m_instance->loadSprite(_name, _spriteCount, _spritesPerRow);
            success = m_instance->m_sprites.contains(_name);
            // TO DO : throw an error if could not load the model (or return nullptr idk)
        }
        return m_instance->m_sprites[_name];
    }


    Shader* ResourceManager::getDefault3DShader() {
        return m_instance->m_renderer->getDefault3DShader();
    }
    Shader* ResourceManager::getDefault2DShader() {
        return m_instance->m_renderer->getDefault2DShader();
    }

    void ResourceManager::addMesh(const std::string& _name, const std::string& _path) {
        m_meshes[_name] = ObjImporter::loadFromFile(_path);
    }
    void ResourceManager::addTexture(const std::string& _name, const std::string& _path) {
        m_textures[_name] = new Texture();
        m_textures[_name]->loadFromDDS(stringToWString(_path).c_str());
        Graphics::addTextureToHeap(m_textures[_name]);
    }

    void ResourceManager::loadSprites() {

    }

    void ResourceManager::loadSprite(const std::string& _name, uint _spriteCount, uint _spritesPerRow) {
        addSprite(_name, os::getFileInDirectory(RES_DIR, _name + ".dds"), _spriteCount, _spritesPerRow);
    }
    void ResourceManager::addSprite(const std::string& _name, const std::string& _path, uint _spriteCount, uint _spritesPerRow)
    {
        SpriteSheet* sprite = new SpriteSheet(stringToWString(_path).c_str(), _spriteCount, _spritesPerRow);
        m_sprites[_name] = sprite;
        // Init text rect
        struct Vertex
        {
            float x, y, z, u, v;
        };

        Vertex boxVertex[] = {
            {0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, // Top Left
            {(float)(sprite->spriteSize.x), 0.0f, 0.0f, 1.0f / sprite->size.x, 0.0f}, // Top Right
            {0.0f, -(float)(sprite->spriteSize.y), 0.0f, 0.0f, 1.0f / sprite->size.y},  // Bottom Left
            {(float)(sprite->spriteSize.x), -(float)(sprite->spriteSize.y), 0.0f, 1.0f / sprite->size.x, 1.0f / sprite->size.y} // Bottom Right
        };

        uint boxIndex[] = {
            0, 1, 3,
            3, 2, 0
        };

        Geometry spriteGeo(boxVertex, 4, boxIndex, 6, Geometry::_2D);
        sprite->mesh = new StaticMesh(spriteGeo);

        sprite->mat = new Material(getDefault2DShader());
        sprite->mat->setProperty("textureID", sprite->texture.getId());
        sprite->mat->setProperty("color", DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
    }
}