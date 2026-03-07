#pragma once

#include <DirectXMath.h>
#include <vector>
#include <string>

// Forward declaration
namespace angler {
    class StaticMesh;
}

namespace angler {

    class ObjImporter {
    public:
        static StaticMesh* loadFromFile(const std::string& path);

    private:
        struct Vertex {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT4 color;
            DirectX::XMFLOAT2 uv;
            DirectX::XMFLOAT3 normal;
        };
        
        static std::vector<std::string> splitString(const std::string& str, char delimiter);
        static float toFloat(const std::string& str);
        static int toInt(const std::string& str);
    };
}