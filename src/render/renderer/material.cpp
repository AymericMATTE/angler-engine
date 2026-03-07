#include "pch.h"
#include "material.h"

#include <dxcapi.h>
#include <d3d12shader.h>

#include "dx12/graphics/graphics.h"

namespace angler {

    Material::Material(Shader* _shader) : m_shader(_shader){
        if (m_shader)
            reflectShader();
    }

    Shader* Material::getShader() const {
        return m_shader;
    }

    DynamicConstantBuffer* Material::getConstantBuffer() const {
        return &m_cb;
    }

    void Material::reflectShader() {
        IDxcResult* result = m_shader->getPixelResult();

        IDxcBlob* reflectionBlob;
        ThrowIfFailed(result->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&reflectionBlob), nullptr));

        const DxcBuffer reflectionBuffer
        {
            .Ptr = reflectionBlob->GetBufferPointer(),
            .Size = reflectionBlob->GetBufferSize(),
            .Encoding = 0,
        };

        ID3D12ShaderReflection* reflector = nullptr;
        HRESULT hr = Graphics::GetDXCUtils()->CreateReflection(&reflectionBuffer, IID_PPV_ARGS(&reflector));

        if (FAILED(hr) || !reflector)
            return;

        D3D12_SHADER_DESC shaderDesc;
        reflector->GetDesc(&shaderDesc);

        UINT cbCount = shaderDesc.ConstantBuffers;

        for (UINT i = 0; i < cbCount; ++i) {
            ID3D12ShaderReflectionConstantBuffer* cb =
                reflector->GetConstantBufferByIndex(i);

            D3D12_SHADER_BUFFER_DESC cbDesc;
            cb->GetDesc(&cbDesc);
            
            D3D12_SHADER_INPUT_BIND_DESC bindDesc;
            reflector->GetResourceBindingDescByName(cbDesc.Name, &bindDesc);

            // I only care about shit in register (b1, space1)
            if (bindDesc.BindPoint != 1 || bindDesc.Space != 1)
                continue;

            m_bufferSize = cbDesc.Size;
            m_cpuBuffer.resize(m_bufferSize);

            m_cb.init(m_bufferSize);

            for (UINT v = 0; v < cbDesc.Variables; ++v) {
                ID3D12ShaderReflectionVariable* var =
                    cb->GetVariableByIndex(v);

                D3D12_SHADER_VARIABLE_DESC varDesc;
                var->GetDesc(&varDesc);

                PropertyInfo info;
                info.offset = varDesc.StartOffset;
                info.size   = varDesc.Size;

                m_layout[varDesc.Name] = info;
            }

            break; // Found b2, no need to continue
        }

        reflector->Release();
    }

    void Material::bind() {
        if (!m_shader)
            return;
        
        uint size = static_cast<uint>(m_cpuBuffer.size()) * sizeof(byte);
        
        m_cb.init(size);
        m_cb.copyData(m_cpuBuffer.data(), 0, size);
    }

    Material& Material::getDefault() {
        static Material defaultMat(nullptr);
        return defaultMat;
    }
}