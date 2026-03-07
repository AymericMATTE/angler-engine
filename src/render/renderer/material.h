#pragma once

// Local dependencies
#include "../dx12/shaders/shader.h"
#include "../dx12/buffers/dynamic-constant-buffer.h"

// External dependencies
#include <unordered_map>
#include <vector>
#include <string>

namespace angler {
    class Material {
    public:
        Material(Shader* _shader);
        ~Material() = default;

        Shader* getShader() const;
        DynamicConstantBuffer* getConstantBuffer() const;

        template<typename T>
        void setProperty(const std::string& _name, const T& _value);

        void bind();

        static Material& getDefault(); // TODO: resource manager later

    private:
        void reflectShader(); // builds layout from shader

        struct PropertyInfo {
            uint32_t offset;
            uint32_t size;
        };

        Shader* m_shader;
        mutable DynamicConstantBuffer m_cb;

        std::unordered_map<std::string, PropertyInfo> m_layout;

        std::vector<byte> m_cpuBuffer;
        uint32_t m_bufferSize = 0;
    };
}
#include "material.inl"