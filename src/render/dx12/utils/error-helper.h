#pragma once

// Local dependencies
#include "common/types.h"

// External dependencies
#include <string>
#include <intsafe.h>
#include <stringapiset.h>

class DxException {
public:
    DxException() = default;
    DxException(HRESULT _hr, const std::wstring& _functionName, const std::wstring& _filename, int _lineNumber);

    [[nodiscard]] std::wstring toString()const;

    HRESULT errorCode = S_OK;
    std::wstring functionName;
    std::wstring filename;
    int lineNumber = -1;
};

angler::uint getBytesPerPixel(DXGI_FORMAT _format);

inline std::wstring ansiToWString(const std::string& _str) {
    WCHAR buffer[512];
    MultiByteToWideChar(CP_ACP, 0, _str.c_str(), -1, buffer, 512);
    return std::wstring(buffer);
}

#ifndef ThrowIfFailed
#define ThrowIfFailed(x) {                                              \
                                                                        \
    HRESULT hr__ = (x);                                                 \
    std::wstring wfn = ansiToWString(__FILE__);                         \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); }   \
}
#endif

#define DX12_OBJ_RELEASE(x) if(x) { x->Release(); x = nullptr; }