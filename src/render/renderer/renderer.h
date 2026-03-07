#pragma once

// Local dependencies
#include "light.h"
#include "material.h"
#include "object-buffer-pool.h"
#include "../dx12/shaders/shader.h"
#include "../dx12/surfaces/swap-chain.h"
#include "../dx12/objects/sprite-sheet.h"
#include "../dx12/surfaces/depth-stencil.h"
#include "../dx12/shaders/compute-shader.h"
#include "../dx12/buffers/dynamic-constant-buffer.h"
#include "../dx12/root-signatures/compute-root-sig.h"

// External dependencies
#include <DirectXMath.h>

// Forward declarations
namespace angler {
    class Camera3D;
    class Camera2D;
    class Window;
    class Mesh;
    class StaticMesh;
    class ParticlesEmitter;
}

namespace angler {
    // =================================================================================
    // Defaults
    // =================================================================================
    
    constexpr uint MAX_3D_OBJECTS = 256;
    constexpr uint MAX_3D_LIGHTS = 32;
    constexpr uint MAX_2D_OBJECTS = 256;
    
    constexpr uint DEFAULT_MSAA_SAMPLE_COUNT = 4;
    constexpr uint DEFAULT_MSAA_QUALITY = 0;
    
    constexpr uint MAX_BLOOM_PASSES = 8;
    constexpr float BLOOM_RESOLUTION_SCALE = 0.5;
    
    
    // =================================================================================
    // Others (Will probably move into another file, or refactor)
    // =================================================================================
    
    struct Vertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 uv;
        DirectX::XMFLOAT3 normal;
    };
    struct UIDrawCall {
        const Mesh* mesh;
        DirectX::XMFLOAT4X4 world;
        DirectX::XMFLOAT4X4 texTransform;
        Material* material;
        int layer;
    };
    

    // =================================================================================
    // Passes Data
    // =================================================================================
    
    struct Pass3D {
        DirectX::XMFLOAT4X4 view;
        DirectX::XMFLOAT4X4 proj;
        DirectX::XMFLOAT3 cameraPos;
        float padding0;
        Light lights[MAX_3D_LIGHTS];
        uint lightsCount;
        DirectX::XMFLOAT3 ambientColor;
        float time;
    };
    
    struct Pass2D {
        DirectX::XMFLOAT4X4 proj;
    };
    
    struct PassBloom {
        uint textureID;
        uint extractColors;
        uint horizontalPass;
        uint verticalPass;
        DirectX::XMFLOAT2 texelSize;
        float padding;
    };


    // =================================================================================
    // Pipelines
    // =================================================================================
    
    struct Pipeline3D {
        Shader shader;
        Shader unlitShader;
        Shader heightmapShader;
        InputLayout layout;
        GraphicsRootSig rootSig;
    };
    
    struct PipelineParticles {
        Shader shader;
        ComputeShader computeShader;
        GraphicsRootSig rootSig;
        ComputeRootSig computeRootSig;
    };
    
    struct Pipeline2D {
        Shader shader;
        InputLayout layout;
        GraphicsRootSig rootSig;
    };
    
    struct PipelinePostProcess {
        Shader bloomShader;
        InputLayout layout;
        GraphicsRootSig rootSig;
        StaticMesh* fullscreenMesh = nullptr;
    };
    

    // =================================================================================
    // States
    // =================================================================================

    struct Pass3DState {
        Pass3D data{};
        Frustum frustum;
        DynamicConstantBuffer passCB;
        ObjectBufferPool<MAX_3D_OBJECTS> objects;
        DepthStencil depthStencil;
    };
    
    struct Pass2DState {
        Pass2D data{};
        DynamicConstantBuffer passCB;
        ObjectBufferPool<MAX_2D_OBJECTS> objects;
        DepthStencil depthStencil;
        std::vector<UIDrawCall> drawQueue;
    };
    
    struct FontState {
        SpriteSheet* spriteSheet = nullptr;
        Material* material = nullptr;
        Mesh* mesh = nullptr;
    };
    
    struct PostProcessState {
        PassBloom bloomData{};
        ObjectBufferPool<2 + (2 * MAX_BLOOM_PASSES)> bloomObjects;
        RenderTarget bloomTargetA;
        RenderTarget bloomTargetB;
    };

    
    // =================================================================================
    // MSAA Render Target
    // =================================================================================
    
    struct MSAARenderTarget {
        RenderTarget msaaTarget;      // Multi-sampled render target
        RenderTarget resolveTarget;   // Resolved target x1 samples
        uint sampleCount = 1;
        uint quality = 0;
        bool enabled = false;

        void initialize(uint width, uint height, uint samples, uint sampleQuality, float clearColor[4]);
        void resolve() const;
    };
    
    
    // =================================================================================
    // Renderer
    // =================================================================================
    
    /// <summary>
    /// Provides the primary rendering functionality for Angler.
    /// <para>
    /// Use the Renderer class for all rendering operations. Only access lower-level
    /// functionality (such as Graphics) when absolutely necessary.
    /// </para>
    /// </summary>
    class Renderer {
    public:
        Renderer(const Window& _window, float _renderScale = 1.0f, bool _vsync = true, uint _msaaSamples = DEFAULT_MSAA_SAMPLE_COUNT);
        ~Renderer();
        
        
        // =================================================================================
        // Renderer API
        // =================================================================================
        
        /// <summary>
        /// Begins a new frame.
        /// <para> Must be called before any draw operations.</para>
        /// </summary>
        void beginFrame();
        /// <summary>
        /// End the current frame.
        /// <para> No draw operations should be called after this method.</para>
        /// </summary>
        void endFrame();

        /// <summary>
        /// Begins the 3D rendering pass with the specified camera.
        /// <para> Must be called before drawing any mesh/particle.</para>
        /// </summary>
        /// <param name="_camera">The camera defining the view frustum.</param>
        void begin3D(Camera3D& _camera);
        /// <summary>
        /// Ends the 3D rendering pass. 
        /// <para> No mesh/particle should be drawn after this method.</para>
        /// </summary>
        void end3D();
        
        /// <summary>
        /// Begins the 2D world-space rendering pass with the specified camera.
        /// <para> Must be called drawing before any sprite.</para>
        /// </summary>
        /// <param name="_camera">The orthographic camera for 2D coordinate mapping.</param>
        /// <remarks>Do not use this method, it is not implemented.</remarks>
        void begin2D(Camera2D& _camera);
        /// <summary>
        /// Ends the 2D world-space rendering pass.
        /// <para> No sprite should be drawn after this method.</para>
        /// </summary>
        /// <remarks>Do not use this method, it is not implemented.</remarks>
        void end2D();
        
        /// <summary>
        /// Begins screen-space UI rendering. 
        /// <para> Must be called before drawing any text/sprite.</para>
        /// </summary>
        /// <param name="_scale">Global scale factor for all UI in this pass.</param>
        void beginUI(float _scale = 1.0f);
        /// <summary>
        /// Ends the UI rendering pass.
        /// <para> No text/sprite should be drawn after this method.</para>
        /// </summary>
        void endUI();

        /// <summary>
        /// Illuminate the scene using specified settings this frame.
        /// <para> WARNING : This method should only be executed AFTER 'beginFrame', but BEFORE 'begin3D'.</para>
        /// </summary>
        /// <param name="_light">The light parameters (position, color, range, etc.).</param>
        /// <remarks>Maximum 32 lights per frame. Excess lights are ignored.</remarks>
        void illuminate(Light& _light);
        /// <summary>
        /// Sets the ambient light color for the scene.
        /// <para> In other terms, the color of shadows where there are no lights.</para>
        /// </summary>
        /// <param name="_color">RGB values for the base illumination level.</param>
        void setAmbientColor(DirectX::XMFLOAT3 _color);

        /// <summary>
        /// Draws a mesh in 3D space with the specified transform and material.
        /// </summary>
        /// <param name="_mesh">The geometry to render.</param>
        /// <param name="_transform">Transform object used on the mesh (position, rotation, scale).</param>
        /// <param name="_material">Surface properties. Uses default material if not specified.</param>
        void draw(const Mesh& _mesh, Transform& _transform, Material& _material = Material::getDefault()); // TO DO : Change for an actual Transform
        /// <summary>
        /// Draws text on screen using the current font.
        /// </summary>
        /// <param name="_text">String to display onscreen.</param>
        /// <param name="_screenPos">Position in pixels from top-left.</param>
        /// <param name="_layer">Sorting layer, higher values means farther away from the screen.</param>
        /// <param name="_anchor">Pivot point, relative to the size of the text.</param>
        /// <param name="_scale">Text size multiplier.</param>
        /// <param name="_rotation">Rotation in radians around the anchor point.</param>
        /// <remarks>Must call 'setFont' before use.</remarks>
        void drawText(const char* _text, DirectX::XMFLOAT2 _screenPos, int _layer, 
            DirectX::XMFLOAT2 _anchor = {0,0}, DirectX::XMFLOAT2 _scale = {1,1}, float _rotation = 0, DirectX::XMFLOAT4 _color = {1, 1, 1, 1});
        /// <summary>
        /// Draws a sprite from a sprite-sheet on screen.
        /// </summary>
        /// <param name="_spriteSheet">Texture atlas containing the sprite frames.</param>
        /// <param name="_screenPos">Position in pixels from top-left.</param>
        /// <param name="_layer">Sorting layer, higher values means farther away from the screen.</param>
        /// <param name="_anchor">Pivot point, relative to the size of the sprite.</param>
        /// <param name="_scale">Sprite size multiplier.</param>
        /// <param name="_rotation">Rotation in radians around the anchor point.</param>
        void drawSprite(SpriteSheet* _spriteSheet, DirectX::XMFLOAT2 _screenPos, int _layer,
            DirectX::XMFLOAT2 _anchor = {0,0}, DirectX::XMFLOAT2 _scale = {1,1}, float _rotation = 0, DirectX::XMFLOAT4 _color = {1, 1, 1, 1});
        /// <summary>
        /// Draws a particle system.
        /// </summary>
        /// <param name="_emitter">The particle emitter to simulate and render.</param>
        void drawEmitter(ParticlesEmitter& _emitter);
        
        /// <summary>
        /// Sets the font for next 'drawText' calls.
        /// </summary>
        /// <param name="_font">Sprite sheet containing the font glyphs.</param> // Yeah 'glyphs' I'm fancy :)
        void setFont(SpriteSheet* _font);
        
        
        // =================================================================================
        // Time setters, (Necessary for particles & some shaders to work)
        // =================================================================================

        void setDeltaTime(float _delta);
        
        
        // =================================================================================
        // MSAA Controls (Not implemented yet, or at all...)
        // =================================================================================
        
        void setMSAAEnabled(bool _enabled);
        bool isMSAAEnabled() const;
        void setMSAASampleCount(uint _samples);
        uint getMSAASampleCount() const;

        
        // =================================================================================
        // Accessors : TEMP (need Shaders to be managed in ResourceManager)
        // =================================================================================
        
        Shader* getDefault3DShader();
        Shader* getDefault3DShaderUnlit();
        Shader* getDefaultParticlesShader();
        Shader* getDefaultHeightmapShader();
        Shader* getDefault2DShader();
        ComputeShader* getDefaultParticleShader();
        
        GraphicsRootSig* get3DRootSig();
        InputLayout* get3DInputLayout();

    private:
        // =================================================================================
        // Broad Renderer-related Members
        // =================================================================================
        
        const Window& m_window;
        SwapChain m_swapChain;
        float m_renderScale;
        bool m_useVsync;
        uint m_msaaSamples;
        uint m_msaaQuality;
        
        float m_deltaTime;
        float m_time;

        // =================================================================================
        // State-related Members
        // =================================================================================
        Pipeline3D m_3dPipeline;
        Pass3DState m_3dPass;

        PipelineParticles m_particlePipeline;

        Pipeline2D m_2dPipeline;
        Pass2DState m_2dPass;

        PipelinePostProcess m_postProcess;
        PostProcessState m_postProcessPass;
        
        MSAARenderTarget m_sceneTarget;

        FontState m_font;
        
        
        // =================================================================================
        // Renderer Initialization 
        // =================================================================================
        
        void initSceneTarget(uint _width, uint _height, uint _samples, uint _quality);
        void initPostProcess();
        void init3DPipeline();
        void init2DPipeline();
        void initParticlePipeline();
        void initBuffers(uint _width, uint _height);
        
        
        // =================================================================================
        // Utilities
        // =================================================================================
        
        void submitUI(const Mesh* _mesh, const DirectX::XMFLOAT4X4& _world, 
            const DirectX::XMFLOAT4X4& _texTransform, Material* _material, int _layer);
        DirectX::XMMATRIX buildWorldMatrix2D(DirectX::XMFLOAT2 _screenPos, DirectX::XMFLOAT2 _anchor, 
            DirectX::XMFLOAT2 _scale, float _rotation, int _layer, DirectX::XMFLOAT2 _size) const;
    };
}