#pragma once

#include "common/types.h"
#include "common/platform/windows/win32.h"

#include <string>

namespace angler {

    class Window {
    public:
        Window(std::wstring _title, int _width = 960, int _height = 540, bool _fullscreen = false);
        ~Window();
        
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        void setIcon(const std::wstring& _path);
        void setTitle(const std::wstring& _title);
        void setBackground(const std::wstring& _path, uint32_t _bgColor = 0x00000000);

        bool isOpen() const;

        void pollEvents();
    
        int getX() const;
        int getY() const;
        uint getWidth() const;
        uint getHeight() const;
    
        HWND getNativeHandle() const;

        // Fullscreen functionality
        void setFullscreen(bool _fullscreen);
        void toggleFullscreen();
        bool isFullscreen() const;

    private:
        std::wstring m_title;
        int m_width;
        int m_height;
        int m_x = 0;
        int m_y = 0;
        bool m_opened = false;
        bool m_fullscreen = false;
        
        // Utilities
        void computeWindowRect();
        HBITMAP loadBMP(const std::wstring& _path);
        RECT calculateFitRect(int _srcW, int _srcH, int _dstW, int _dstH) const;
        HBITMAP createCenteredBitmap(HBITMAP _source, int _width, int _height, uint32_t _bgColor);

        // Saved window state for restoring from fullscreen
        int m_windowedX = 0;
        int m_windowedY = 0;
        int m_windowedWidth = 960;
        int m_windowedHeight = 540;
        DWORD m_windowedStyle = 0;

        // Native Windows stuff
        HBITMAP m_bgBitmap = nullptr;
        HBRUSH m_bgBrush = nullptr;
        HWND m_handle = nullptr;
        HMODULE m_hInst = nullptr;
        WNDCLASS m_winCls{};
        MSG m_msg{};

        // Message handling
        LRESULT windowProc(HWND _hwnd, UINT _message, WPARAM _wParam, LPARAM _lParam);
        static LRESULT CALLBACK procInitializer(HWND _hwnd, UINT _message, WPARAM _wParam, LPARAM _lParam);
    };
}