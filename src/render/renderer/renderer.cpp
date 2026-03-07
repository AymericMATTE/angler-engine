#include "pch.h"
#include "renderer.h"

// Local dependencies
#include "../os/window.h"
#include "cameras/camera-3d.h"
#include "../dx12/objects/mesh.h"
#include "../dx12/objects/geometry.h"
#include "../dx12/graphics/graphics.h"
#include "../dx12/objects/static-mesh.h"
#include "../dx12/particles-emitter/particles-emitter.h"

// External dependencies
#include <algorithm>
#include <ctime>

#include "common.h"
#include "utils/os.h"

namespace angler {
    // =================================================================================
    // MSAA Render Target
    // =================================================================================
    
    void MSAARenderTarget::initialize(uint width, uint height, uint samples, uint sampleQuality, float clearColor[4]) {
        sampleCount = samples;
        quality = sampleQuality;
        enabled = (samples > 1);

        if (!enabled) {
            resolveTarget.initialize(width, height, clearColor);
            return;
        }

        // Check for support
        if (Graphics::getMSAAQualityLevels(samples) == 0) {
            enabled = false;
            resolveTarget.initialize(width, height, clearColor);
            return;
        }

        float msaaClear[4] = { 0, 0, 0, 1 };
        if (clearColor) {
            memcpy(msaaClear, clearColor, sizeof(float) * 4);
        }

        // Create MSAA RenderTarget
        msaaTarget.initializeMSAA(width, height, samples, quality, msaaClear);

        // Create resolve target (non-MSAA)
        resolveTarget.initialize(width, height, clearColor);
    }

    void MSAARenderTarget::resolve() const {
        if (!enabled || sampleCount <= 1) return;
    
        // Resolve MSAA to non-MSAA target
        Graphics::resolveMSAAResource(
            msaaTarget.getResource(),
            resolveTarget.getResource(),
            Graphics::getRenderTargetFormat()
        );
    }

    
    // =================================================================================
    // Renderer Initialization 
    // =================================================================================

    Renderer::Renderer(const Window& _window, float _renderScale, bool _vsync, uint _msaaSamples)
        : m_window(_window), m_renderScale(_renderScale), m_useVsync(_vsync),
          m_msaaSamples(_msaaSamples), m_msaaQuality(DEFAULT_MSAA_QUALITY) {
        
        // Init time vars to 0
        m_deltaTime = 0.0f;
        m_time = 0.0f;
        
        srand(static_cast<uint>(time(NULL)));

        const uint width = static_cast<uint>(_window.getWidth() * _renderScale);
        const uint height = static_cast<uint>(_window.getHeight() * _renderScale);
        
        initSceneTarget(width, height, _msaaSamples, DEFAULT_MSAA_QUALITY);
        initPostProcess();
        init3DPipeline();
        initParticlePipeline();
        init2DPipeline();
        initBuffers(width, height);
    }
    
    Renderer::~Renderer() {
        m_3dPass.objects.cleanup();
        m_2dPass.objects.cleanup();
        delete m_postProcess.fullscreenMesh;
    }

    void Renderer::initSceneTarget(uint _width, uint _height, uint _samples, uint _quality) {
        float clearColor[4]{0.0f, 0.0f, 0.0f, 1.0f};
        m_sceneTarget.initialize(_width, _height, _samples, _quality, clearColor);

        // Register resolve target as SRV
        Graphics::getSRVHeap()->addShaderResourceView(&m_sceneTarget.resolveTarget);
    }

    void Renderer::initPostProcess() {
        m_postProcess.rootSig.initialize();

        m_postProcess.layout.add<DirectX::XMFLOAT2>("POSITION");
        m_postProcess.layout.add<DirectX::XMFLOAT2>("TEXCOORD");
        m_postProcess.bloomShader.initialize(
            stringToWString(os::getFileInDirectory(RES_DIR,"bloom.hlsl")).c_str(),
            stringToWString(os::getFileInDirectory(RES_DIR,"bloom.hlsl")).c_str(),
            &m_postProcess.rootSig,
            &m_postProcess.layout
        );
        
        
        
        float clearColor[4]{0.0f, 0.0f, 0.0f, 1.0f};
        
        m_postProcessPass.bloomTargetA.initialize(
            static_cast<uint>(m_window.getWidth() * m_renderScale * BLOOM_RESOLUTION_SCALE),
            static_cast<uint>(m_window.getHeight() * m_renderScale * BLOOM_RESOLUTION_SCALE),
                clearColor
        );
        Graphics::getSRVHeap()->addShaderResourceView(&m_postProcessPass.bloomTargetA);
        
        m_postProcessPass.bloomTargetB.initialize(
            static_cast<uint>(m_window.getWidth() * m_renderScale * BLOOM_RESOLUTION_SCALE),
            static_cast<uint>(m_window.getHeight() * m_renderScale * BLOOM_RESOLUTION_SCALE),
                clearColor
        );
        Graphics::getSRVHeap()->addShaderResourceView(&m_postProcessPass.bloomTargetB);

        struct QuadVertex { DirectX::XMFLOAT2 pos; DirectX::XMFLOAT2 uv; };
        QuadVertex vertices[4] = {
            {{-1,  1}, {0, 0}},
            {{ 1,  1}, {1, 0}},
            {{ 1, -1}, {1, 1}},
            {{-1, -1}, {0, 1}}
        };
        uint32_t indices[6] = { 0, 1, 3, 3, 1, 2 };
        
        Geometry quadGeo(vertices, sizeof(QuadVertex), 4, indices, 6, Geometry::_2D);
        m_postProcess.fullscreenMesh = new StaticMesh(quadGeo);
    }

    void Renderer::init3DPipeline() {
        m_3dPipeline.rootSig.addConstantBufferView(0);
        m_3dPipeline.rootSig.addConstantBufferView(1);
        m_3dPipeline.rootSig.initialize();

        m_3dPipeline.layout.add<DirectX::XMFLOAT3>("POSITION");
        m_3dPipeline.layout.add<DirectX::XMFLOAT4>("COLOR");
        m_3dPipeline.layout.add<DirectX::XMFLOAT2>("TEXCOORD");
        m_3dPipeline.layout.add<DirectX::XMFLOAT3>("NORMAL");

        m_3dPipeline.shader.initialize(
        stringToWString(os::getFileInDirectory(RES_DIR,"3d-lit.hlsl")).c_str(),
        stringToWString(os::getFileInDirectory(RES_DIR,"3d-lit.hlsl")).c_str(),
            &m_3dPipeline.rootSig,
            &m_3dPipeline.layout
        );
        m_3dPipeline.shader.setAlpha(false);

        m_3dPipeline.unlitShader.initialize(
        stringToWString(os::getFileInDirectory(RES_DIR,"3d-unlit.hlsl")).c_str(),
        stringToWString(os::getFileInDirectory(RES_DIR,"3d-unlit.hlsl")).c_str(),
            &m_3dPipeline.rootSig,
            &m_3dPipeline.layout
        );
        m_3dPipeline.unlitShader.setAlpha(false);
        
        m_3dPipeline.heightmapShader.initialize(
        stringToWString(os::getFileInDirectory(RES_DIR,"heightmap.hlsl")).c_str(),
        stringToWString(os::getFileInDirectory(RES_DIR,"heightmap.hlsl")).c_str(),
            &m_3dPipeline.rootSig,
            &m_3dPipeline.layout
        );
        m_3dPipeline.unlitShader.setAlpha(false);
    }

    void Renderer::initParticlePipeline() {
        m_particlePipeline.rootSig.addShaderResourceView(0);
        m_particlePipeline.rootSig.addConstantBufferView(0);
        m_particlePipeline.rootSig.initialize();

        m_particlePipeline.shader.initialize(
        stringToWString(os::getFileInDirectory(RES_DIR,"particles-draw.hlsl")).c_str(),
        stringToWString(os::getFileInDirectory(RES_DIR,"particles-draw.hlsl")).c_str(),
            &m_particlePipeline.rootSig,
            &m_3dPipeline.layout
        );
        
        m_particlePipeline.computeRootSig.addConstantBufferView(0);
        m_particlePipeline.computeRootSig.addUnorderedAccessView(0);
        m_particlePipeline.computeRootSig.initialize();
        
        m_particlePipeline.computeShader.initialize(
            stringToWString(os::getFileInDirectory(RES_DIR,"particle.hlsl")).c_str(),
            &m_particlePipeline.computeRootSig
        );
    }

    void Renderer::init2DPipeline() {
        m_2dPipeline.rootSig.addConstantBufferView(0);
        m_2dPipeline.rootSig.addConstantBufferView(1);
        m_2dPipeline.rootSig.initialize();

        m_2dPipeline.layout.add<DirectX::XMFLOAT3>("POSITION");
        m_2dPipeline.layout.add<DirectX::XMFLOAT2>("TEXCOORD");

        m_2dPipeline.shader.initialize(
        stringToWString(os::getFileInDirectory(RES_DIR,"2d-texture.hlsl")).c_str(),
        stringToWString(os::getFileInDirectory(RES_DIR,"2d-texture.hlsl")).c_str(),
            &m_2dPipeline.rootSig,
            &m_2dPipeline.layout
        );
        m_2dPipeline.shader.setAlpha(true);
    }

    void Renderer::initBuffers(uint _width, uint _height) {
        m_swapChain.initialize(m_window.getNativeHandle(), _width, _height);

        // Make DSB match MSAA used
        if (m_sceneTarget.enabled)
            m_3dPass.depthStencil.initialize(_width, _height, m_msaaSamples, m_msaaQuality);
        else
            m_3dPass.depthStencil.initialize(_width, _height);
        
        m_2dPass.depthStencil.initialize(_width, _height);
        
        Graphics::setDepthState(true);
        Graphics::setDepthStencil(&m_3dPass.depthStencil);

        m_3dPass.data.ambientColor = {0.075f, 0.075f, 0.15f}; // TO DO : Set to a constexpr
        m_3dPass.passCB.init(sizeof(Pass3D));
        m_3dPass.objects.init(2 * sizeof(DirectX::XMFLOAT4X4));

        m_2dPass.passCB.init(sizeof(Pass2D));
        m_2dPass.objects.init(2 * sizeof(DirectX::XMFLOAT4X4));
        
        m_postProcessPass.bloomObjects.init(sizeof(PassBloom));
    }

    
    // =================================================================================
    // Frame Lifecycle
    // =================================================================================

    void Renderer::beginFrame() {
        // Compute time
        m_time += m_deltaTime;
        
        // Switch to MSAA target, only if MSAA is enable
        if (m_sceneTarget.enabled) {
            // Make sure MSAA target is an RT before using it
            Graphics::transitionGraphicsResource(
                &m_sceneTarget.msaaTarget,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,  // Should be the previous state, I guess...
                D3D12_RESOURCE_STATE_RENDER_TARGET
            );
            Graphics::setRenderTarget(&m_sceneTarget.msaaTarget, &m_3dPass.depthStencil);
        } else {
            Graphics::setRenderTarget(&m_sceneTarget.resolveTarget);
        }
        Graphics::beginDrawing();
    }

    void Renderer::endFrame() {
        Graphics::endDrawing();

        m_swapChain.present(m_useVsync);
    
        m_3dPass.objects.reset();
        m_3dPass.data.lightsCount = 0;
        m_2dPass.objects.reset();
        m_postProcessPass.bloomObjects.reset();
    }

    
    // =================================================================================
    // 3D Pass
    // =================================================================================

    void Renderer::begin3D(Camera3D& _camera) {
        const float aspect = static_cast<float>(m_window.getWidth()) / static_cast<float>(m_window.getHeight());
        _camera.update(aspect);

        Graphics::setShader(&m_3dPipeline.shader);

        m_3dPass.data.proj = _camera.getProj();
        m_3dPass.data.view = _camera.getView();
        m_3dPass.data.cameraPos = _camera.getPosition();
        m_3dPass.data.time = m_time;
        m_3dPass.frustum = _camera.getFrustum();
        
        m_3dPass.passCB.copyData(&m_3dPass.data, 0);
        Graphics::setPassData(&m_3dPass.passCB);
    }

    void Renderer::end3D() {
        // Perform MSAA resolving, only if MSAA is enabled
        if (m_sceneTarget.enabled) {
            /// Note : IDK why i need to transition the RT's into a premade state,
            /// From my understanding, it's because resolving is hardware implemented.
            /// Some resources I used :
            /// https://www.gamedev.net/forums/topic/717265-dx12-msaa-help/
            /// https://www.braynzarsoft.net/viewquestion/q33526-adding-msaa-in-dx12?utm_source=chatgpt.com
            /// https://stackoverflow.com/questions/52472333/dx12-open-4x-msaa-failed?utm_source=chatgpt.com
            /// As well as a touch of AI, because to be honest, i found it hardcore to get it working.
            /// I mean, now that i understand it, it seems simple, but the implementation was tough
            Graphics::transitionGraphicsResource(
                &m_sceneTarget.msaaTarget,
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_RESOLVE_SOURCE
            );
            Graphics::transitionGraphicsResource(
                &m_sceneTarget.resolveTarget,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                D3D12_RESOURCE_STATE_RESOLVE_DEST
            );
        
            // Perform resolve
            Graphics::resolveMSAAResource(
                m_sceneTarget.msaaTarget.getResource(),
                m_sceneTarget.resolveTarget.getResource(),
                Graphics::getRenderTargetFormat()
            );
        
            // Transition resolve target to shader resource for post-process
            Graphics::transitionGraphicsResource(
                &m_sceneTarget.resolveTarget,
                D3D12_RESOURCE_STATE_RESOLVE_DEST,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
            );
            
            // Set the texture to sample to be the resolved target
            m_postProcessPass.bloomData.textureID = m_sceneTarget.resolveTarget.getId();
        }
        else {
            // Transition resolve target to shader resource for post-process
            Graphics::transitionGraphicsResource(
                &m_sceneTarget.resolveTarget,
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
            );
            
            // Set the texture to sample to be the resolved target
            m_postProcessPass.bloomData.textureID = m_sceneTarget.resolveTarget.getId();
        }
        
        // Start post process
        
        m_postProcessPass.bloomData.texelSize = {1.0f / (m_window.getWidth() * 0.5f), 1.0f / (m_window.getHeight() * 0.5f) };
        
        Graphics::setDepthState(false);

        // Extract bright colors
        m_postProcessPass.bloomData.extractColors = 1;
        m_postProcessPass.bloomData.verticalPass = 0;
        m_postProcessPass.bloomData.horizontalPass = 0;
        
        Graphics::transitionGraphicsResource(
            &m_postProcessPass.bloomTargetB,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_RENDER_TARGET
        );
    
        Graphics::setRenderTarget(&m_postProcessPass.bloomTargetB, false); // No prev transition, already done above

        Graphics::setShader(&m_postProcess.bloomShader);
        DynamicConstantBuffer* buffer = m_postProcessPass.bloomObjects.next();
        buffer->copyData(&m_postProcessPass.bloomData, 0);
        Graphics::setPassData(buffer);
        Graphics::drawMesh(m_postProcess.fullscreenMesh);
        
        Graphics::transitionGraphicsResource(
            &m_postProcessPass.bloomTargetB,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );

        for (int i = 0; i < MAX_BLOOM_PASSES; ++i) {
            // Horizontal pass
            m_postProcessPass.bloomData.extractColors  = 0;
            m_postProcessPass.bloomData.horizontalPass = 1;
            m_postProcessPass.bloomData.verticalPass   = 0;
            m_postProcessPass.bloomData.textureID      = m_postProcessPass.bloomTargetB.getId();
            
            Graphics::transitionGraphicsResource(
                &m_postProcessPass.bloomTargetA,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                D3D12_RESOURCE_STATE_RENDER_TARGET
            );

            Graphics::setRenderTarget(&m_postProcessPass.bloomTargetA, false);

            Graphics::setShader(&m_postProcess.bloomShader);

            DynamicConstantBuffer* bufferH = m_postProcessPass.bloomObjects.next();
            bufferH->copyData(&m_postProcessPass.bloomData, 0);
            Graphics::setPassData(bufferH);

            Graphics::drawMesh(m_postProcess.fullscreenMesh);

            Graphics::transitionGraphicsResource(
                &m_postProcessPass.bloomTargetA,
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
            );

            // Vertical pass
            m_postProcessPass.bloomData.extractColors  = 0;
            m_postProcessPass.bloomData.horizontalPass = 0;
            m_postProcessPass.bloomData.verticalPass   = 1;
            m_postProcessPass.bloomData.textureID      = m_postProcessPass.bloomTargetA.getId();

            Graphics::transitionGraphicsResource(
                &m_postProcessPass.bloomTargetB,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                D3D12_RESOURCE_STATE_RENDER_TARGET
            );

            Graphics::setRenderTarget(&m_postProcessPass.bloomTargetB, false);

            Graphics::setShader(&m_postProcess.bloomShader);

            DynamicConstantBuffer* bufferV = m_postProcessPass.bloomObjects.next();
            bufferV->copyData(&m_postProcessPass.bloomData, 0);
            Graphics::setPassData(bufferV);

            Graphics::drawMesh(m_postProcess.fullscreenMesh);

            Graphics::transitionGraphicsResource(
                &m_postProcessPass.bloomTargetB,
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
            );
        }

        // Additive blend bloom with sceneColor in backbuffer
        m_postProcessPass.bloomData.extractColors = 0;
        m_postProcessPass.bloomData.verticalPass = 0;
        m_postProcessPass.bloomData.horizontalPass = 0;
        m_postProcessPass.bloomData.textureID = m_postProcessPass.bloomTargetB.getId();

        Graphics::setRenderTarget(m_swapChain.getCurrentRenderTarget());

        Graphics::setShader(&m_postProcess.bloomShader);
        DynamicConstantBuffer* buffer2 = m_postProcessPass.bloomObjects.next();
        buffer2->copyData(&m_postProcessPass.bloomData, 0);
        Graphics::setPassData(buffer2);
        Graphics::drawMesh(m_postProcess.fullscreenMesh);

        Graphics::setDepthState(true);
    }

    void Renderer::illuminate(Light& _light) {
        if (m_3dPass.data.lightsCount < MAX_3D_LIGHTS) {
            m_3dPass.data.lights[m_3dPass.data.lightsCount++] = _light;
        }
    }

    void Renderer::setAmbientColor(DirectX::XMFLOAT3 _color) {
        m_3dPass.data.ambientColor = _color;
    }

    void Renderer::draw(const Mesh& _mesh, Transform& _transform, Material& _material)
    {
        AABB box = _mesh.getBoundingBox();
        if (Physics::isOnFrustum(box, m_3dPass.frustum, _transform) == false)
            return;

        auto* objCB = m_3dPass.objects.next();
        objCB->init(sizeof(DirectX::XMFLOAT4X4));
        DirectX::XMFLOAT4X4 matrix = _transform.getTransposedWorldMatrix();
        objCB->copyData(&matrix, 0);
        Graphics::setGraphicsBuffer(0, ViewType::CBV, objCB);

        Graphics::setShader(_material.getShader());
        _material.bind();
        Graphics::setGraphicsBuffer(1, ViewType::CBV, _material.getConstantBuffer());

        Graphics::drawMesh(&_mesh);
    }

    void Renderer::drawEmitter(ParticlesEmitter& _emitter) {
        _emitter.update(m_deltaTime);
        
        Graphics::setShader(&m_particlePipeline.shader);
        Graphics::setGraphicsBuffer(0, ViewType::SRV, _emitter.getParticlesData());
        Graphics::setGraphicsBuffer(1, ViewType::CBV, _emitter.getDrawData());

        Graphics::drawMesh(_emitter.getPattern(), _emitter.getParticlesCount());
    }
    
    // =================================================================================
    // 2D Pass
    // =================================================================================

    void Renderer::begin2D(Camera2D& _camera) {
        // TO DO : Implement
        // Update : We don't have the time for this...
    }
    void Renderer::end2D() {}


    void Renderer::beginUI(float _scale) {
        Graphics::setShader(&m_2dPipeline.shader);
        Graphics::setDepthStencil(&m_2dPass.depthStencil);
        Graphics::setDepthState(false);

        DirectX::XMStoreFloat4x4(
            &m_2dPass.data.proj,
            DirectX::XMMatrixOrthographicLH(
                static_cast<float>(m_window.getWidth()),
                static_cast<float>(m_window.getHeight()),
                0.01f, 100.0f
            )
        );

        m_2dPass.passCB.copyData(&m_2dPass.data, 0);
        Graphics::setPassData(&m_2dPass.passCB);
    }

    void Renderer::endUI() {
        auto& queue = m_2dPass.drawQueue;
        std::sort(queue.begin(), queue.end(), 
            [](const UIDrawCall& a, const UIDrawCall& b) { return a.layer < b.layer; });

        for (const auto& call : queue) {
            Graphics::setShader(call.material->getShader());
            call.material->bind();
            Graphics::setGraphicsBuffer(1, ViewType::CBV, call.material->getConstantBuffer());

            auto* objCB = m_2dPass.objects.next();
            objCB->init(2 * sizeof(DirectX::XMFLOAT4X4));
            objCB->copyData(&call.world, 0);
            objCB->copyData(&call.texTransform, sizeof(DirectX::XMFLOAT4X4));
            Graphics::setGraphicsBuffer(0, ViewType::CBV, objCB);

            Graphics::drawMesh(call.mesh);
        }

        queue.clear();
        Graphics::setDepthState(true);
    }

    void Renderer::submitUI(const Mesh* _mesh, const DirectX::XMFLOAT4X4& _world,
        const DirectX::XMFLOAT4X4& _texTransform, Material* _material, int _layer) {
        
        m_2dPass.drawQueue.push_back({_mesh, _world, _texTransform, _material, _layer});
    }

    DirectX::XMMATRIX Renderer::buildWorldMatrix2D(DirectX::XMFLOAT2 _screenPos, DirectX::XMFLOAT2 _anchor,
        DirectX::XMFLOAT2 _scale, float _rotation, int _layer, DirectX::XMFLOAT2 _size) const {
        
        const uint width = m_window.getWidth();
        const uint height = m_window.getHeight();

        DirectX::XMFLOAT3 localOffset(-_anchor.x * _size.x * _scale.x, _anchor.y * _size.y * _scale.y, 0);
        DirectX::XMFLOAT3 position(_screenPos.x - static_cast<float>(width) / 2.0f, -_screenPos.y + static_cast<float>(height) / 2.0f, 0.0f);

        DirectX::XMVECTOR rotQuat = DirectX::XMQuaternionRotationRollPitchYaw(0, 0, _rotation);
        
        DirectX::XMMATRIX mat = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&localOffset))
            * DirectX::XMMatrixRotationQuaternion(rotQuat)
            * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&position));

        DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling(_scale.x, _scale.y, 1.0f);
        mat = scaleMat * mat;

        DirectX::XMMATRIX layerMat = DirectX::XMMatrixTranslation(0, 0, static_cast<float>(_layer));
        mat = layerMat * mat;

        return mat;
    }

    void Renderer::drawText(const char* _text, DirectX::XMFLOAT2 _screenPos, int _layer,
        DirectX::XMFLOAT2 _anchor, DirectX::XMFLOAT2 _scale, float _rotation, DirectX::XMFLOAT4 _color) {
        
        if (!m_font.spriteSheet) return;

        const uint textLen = static_cast<uint>(strlen(_text));
        const DirectX::XMUINT2 charSize = m_font.spriteSheet->spriteSize;
        const DirectX::XMFLOAT2 totalSize(static_cast<float>(textLen * charSize.x), static_cast<float>(charSize.y));

        for (uint i = 0; i < textLen; ++i) {
            char c = _text[i] - 32;
            if (c < 0 || c > static_cast<char>(m_font.spriteSheet->count)) continue;

            const float xOffset = i * charSize.x * _scale.x;
            DirectX::XMMATRIX world = buildWorldMatrix2D(
                {_screenPos.x + xOffset, _screenPos.y},
                _anchor, _scale, _rotation, _layer, totalSize
            );

            DirectX::XMFLOAT4X4 worldT;
            DirectX::XMStoreFloat4x4(&worldT, DirectX::XMMatrixTranspose(world));

            m_font.spriteSheet->SetCurrentFrame(c);
            submitUI(m_font.mesh, worldT, m_font.spriteSheet->getTextureTransform(), m_font.material, _layer);
        }
    }

    void Renderer::drawSprite(SpriteSheet* sprite, DirectX::XMFLOAT2 _screenPos, int _layer,
        DirectX::XMFLOAT2 _anchor, DirectX::XMFLOAT2 _scale, float _rotation, DirectX::XMFLOAT4 _color) {
        
        if (!sprite) return;

        DirectX::XMMATRIX world = buildWorldMatrix2D(
            _screenPos, _anchor, _scale, _rotation, _layer, {static_cast<float>(sprite->spriteSize.x), static_cast<float>(sprite->spriteSize.y)}
        );

        DirectX::XMFLOAT4X4 worldT;
        DirectX::XMStoreFloat4x4(&worldT, DirectX::XMMatrixTranspose(world));

        submitUI(sprite->mesh, worldT, sprite->getTextureTransform(), sprite->mat, _layer);
    }


    void Renderer::setFont(SpriteSheet* _font) {
        m_font.spriteSheet = _font;
        m_font.mesh = _font->mesh;
        m_font.material = _font->mat;
    }
    
    void Renderer::setDeltaTime(float _delta) {
        m_deltaTime = _delta;
    }
    

    // =================================================================================
    // MSAA Controls
    // =================================================================================

    void Renderer::setMSAAEnabled(bool _enabled) {
        // TO DO : Reset render targets
        
        if (_enabled && m_msaaSamples > 1) {
            m_sceneTarget.enabled = true;
        } else {
            m_sceneTarget.enabled = false;
        }
    }

    bool Renderer::isMSAAEnabled() const {
        return m_sceneTarget.enabled;
    }

    void Renderer::setMSAASampleCount(uint _samples) {
        m_msaaSamples = _samples;
        // TO DO : Reset render targets
        
    }

    uint Renderer::getMSAASampleCount() const {
        return m_msaaSamples;
    }

    
    // =================================================================================
    // Accessors : TEMP
    // =================================================================================

    Shader* Renderer::getDefault3DShader() {
        return &m_3dPipeline.shader;
    }
    
    Shader* Renderer::getDefault3DShaderUnlit() {
        return &m_3dPipeline.unlitShader;
    }

    Shader* Renderer::getDefaultParticlesShader()
    {
        return &m_particlePipeline.shader;
    }
    
    Shader* Renderer::getDefaultHeightmapShader() {
        return &m_3dPipeline.heightmapShader;
    }

    Shader* Renderer::getDefault2DShader() {
        return &m_2dPipeline.shader;
    }
    
    ComputeShader* Renderer::getDefaultParticleShader() {
        return &m_particlePipeline.computeShader;
    }
    GraphicsRootSig* Renderer::get3DRootSig() {
        return &m_3dPipeline.rootSig;
    }
    InputLayout* Renderer::get3DInputLayout() {
        return &m_3dPipeline.layout;
    }
}