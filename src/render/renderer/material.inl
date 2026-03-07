#pragma once

namespace angler {
    template<typename T>
    void Material::setProperty(const std::string& _name, const T& _value) {
        if (!m_shader)
            return;

        auto it = m_layout.find(_name);
        if (it == m_layout.end()) { // Properti does not exist in the Shader
            return;
        }

        const PropertyInfo& info = it->second;
        
        if (sizeof(T) > info.size) { // Size of our value to big / wrong type i guess ?
            return;
        }

        memcpy(m_cpuBuffer.data() + info.offset, &_value, sizeof(T));
    }
}