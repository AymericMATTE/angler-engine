#include "pch.h"
#include "error-helper.h"

// External dependencies
#include <comdef.h>

DxException::DxException(HRESULT _hr, const std::wstring & _functionName, const std::wstring & _filename, int _lineNumber) :
    errorCode(_hr), functionName(_functionName), filename(_filename), lineNumber(_lineNumber) {}

std::wstring DxException::toString() const {
    // Get the string description of the error code.
    _com_error err(errorCode);
    std::wstring msg = err.ErrorMessage();

    return functionName + L" failed in " + filename + L"; line " + std::to_wstring(lineNumber) + L"; error: " + msg;
}

UINT getBytesPerPixel(DXGI_FORMAT _format) {
    switch (_format) {
        case DXGI_FORMAT_R8_UNORM:
            return 1;
        case DXGI_FORMAT_R8G8_UNORM:
            return 2;
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            return 4;
        case DXGI_FORMAT_B8G8R8A8_UNORM:
            return 4;
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
            return 8;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            return 16;
        default:
            throw; // TODO LOG
    }
}