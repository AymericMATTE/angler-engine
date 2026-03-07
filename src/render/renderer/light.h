#pragma once

// External dependencies
#include <DirectXMath.h>
#include <common/types.h>

namespace angler {
    enum Type : uint {  // Would like to work with byte, but does not exist in HLSL
        LIGHT_TYPE_DIRECTIONAL  = 0,
        LIGHT_TYPE_POINT        = 1,
        LIGHT_TYPE_SPOT         = 2
    };
    
    /// Just in case someone asks why members are declared like that..
    /// IT'S BECAUSE OF THE FUKING ALIGNMENT !
    ///
    /// Alignment 'Map' : (helped me to set up this mess)
    /// Type type;                      -> 4 bytes
    /// float intensity;                -> 4 bytes
    /// float spotInnerCos;             -> 4 bytes
    /// float spotOuterCos;             -> 4 bytes
    ///                                 
    /// Current total :                 -> 16 bytes
    ///                                 
    /// DirectX::XMFLOAT3 color;        -> 12 bytes
    /// float range;                    -> 4 bytes
    ///                                 
    /// Current total :                 -> 32 bytes
    ///                                 
    /// DirectX::XMFLOAT3 direction;    -> 12 bytes
    /// float padding0;                 -> 4 bytes
    ///         
    /// Current total :                 -> 48 bytes
    /// 
    /// DirectX::XMFLOAT3 position;     -> 12 bytes
    /// float padding1;                 -> 4 bytes
    /// 
    /// Final total size :              -> 64 bytes


    struct Light {
        Type type = LIGHT_TYPE_DIRECTIONAL;
        // Source intensity (dir/point/spot)
        float intensity = 1.0f;
        // Cosine of inner cone (spot only)
        float spotInnerCos = 0.0f;
        // Cosine of outer cone (spot only)
        float spotOuterCos = 0.0f;

        // Source color (dir/point/spot)
        DirectX::XMFLOAT3 color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
        // Attenuation range (point/spot)
        float range = 0.0f;
        
        // Direction TO light (dir/spot)
        DirectX::XMFLOAT3 direction = DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f);
        float padding0 = 0.0f;
        
        // World position (point/spot)
        DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        float padding1 = 0.0f;
        
        void directional();
        void point();
    };
    
    // Thanks ChatGPT, for those assert you saved my life
    static_assert(sizeof(Light) == 64, "Light must be 64 bytes!");
    static_assert(sizeof(Light) % 16 == 0, "Light must be 16-byte aligned!");
}