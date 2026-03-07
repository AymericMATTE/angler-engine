#include "pch.h"
#include "window.h"

#include <platform/windows/win.h>
#include <iostream>

namespace angler {
    
    Window::Window(std::wstring _title, int _width, int _height, bool _fullscreen)
        : m_title(std::move(_title)), 
          m_width(_width), 
          m_height(_height),
          m_windowedWidth(_width),
          m_windowedHeight(_height),
          m_fullscreen(_fullscreen) {
        
        m_hInst = GetModuleHandle(nullptr);

        m_winCls.hCursor = LoadCursor(nullptr, IDC_ARROW); // TO DO : Implement properly in platform::win abstraction
        
        platform::win::Result result = platform::win::RegisterWindowClass(
            &m_winCls,
            m_hInst,
            m_title.c_str(),
            procInitializer
        );

        if (!result.succeeded()) {
            // TO DO : Add proper error handling
        }

        // Calculate initial position (center on screen)
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        m_windowedX = (screenWidth - m_width) / 2;
        m_windowedY = (screenHeight - m_height) / 2;
        m_x = m_windowedX;
        m_y = m_windowedY;

        platform::win::WindowDescriptor desc;
        desc.title = m_title.c_str();
        desc.width = m_width;
        desc.height = m_height;
        desc.x = m_x;
        desc.y = m_y;
        
        // Use borderless style if starting fullscreen
        if (m_fullscreen) {
            desc.style = WS_POPUP; // Borderless
        } else {
            desc.style = WS_OVERLAPPEDWINDOW; // Normal windowed
        }

        result = platform::win::CreateWindowFromClass(
            &m_handle,
            &m_winCls,
            m_hInst,
            desc,
            this
        );

        if (!result.succeeded()) {
            // TO DO : Add proper error handling
        }

        // Apply fullscreen if requested
        if (m_fullscreen) {
            setFullscreen(true);
        }

        ShowWindow(m_handle, SW_SHOW);
        m_opened = true;
    }

    Window::~Window() {
        if (m_bgBrush) {
            DeleteObject(m_bgBrush);
        }
        if (m_bgBitmap) {
            DeleteObject(m_bgBitmap);
        }
        
        if (m_handle) {
            DestroyWindow(m_handle);
            m_handle = nullptr;
        }
        UnregisterClass(m_winCls.lpszClassName, m_hInst);
    }

    void Window::setIcon(const std::wstring& _path) {
        HICON icon = static_cast<HICON>(LoadImageW(
            nullptr,
            _path.c_str(),
            IMAGE_ICON,
            0, 0,
            LR_LOADFROMFILE | LR_DEFAULTSIZE
        ));
        
        if (!icon) return;
        
        SendMessageW(m_handle, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon));
        SendMessageW(m_handle, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon));
    }

    void Window::setTitle(const std::wstring& _title) {
        m_title = _title;
        SetWindowTextW(m_handle, m_title.c_str());
    }
    
    void Window::setBackground(const std::wstring& _path, uint32_t _bgColor) {
        // Cleanup previous background
        if (m_bgBrush) {
            DeleteObject(m_bgBrush);
            m_bgBrush = nullptr;
        }
        if (m_bgBitmap) {
            DeleteObject(m_bgBitmap);
            m_bgBitmap = nullptr;
        }

        // Load BMP file
        HBITMAP hSource = loadBMP(_path);
        if (!hSource) {
            return;
        }

        // Get window size
        RECT clientRect;
        GetClientRect(m_handle, &clientRect);
        int clientW = clientRect.right;
        int clientH = clientRect.bottom;

        // Center bitmap
        m_bgBitmap = createCenteredBitmap(hSource, clientW, clientH, _bgColor);
        DeleteObject(hSource);

        if (!m_bgBitmap) return;

        // Create brush
        m_bgBrush = CreatePatternBrush(m_bgBitmap);
        if (!m_bgBrush) return;

        // Window class BG
        SetClassLongPtr(m_handle, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(m_bgBrush));

        // Force paint
        InvalidateRect(m_handle, nullptr, TRUE);
        UpdateWindow(m_handle);
    }

    bool Window::isOpen() const {
        return m_opened;
    }

    void Window::computeWindowRect() {
        RECT rect{};
        if (GetWindowRect(m_handle, &rect)) {
            m_x = rect.left;
            m_y = rect.top;
            m_width = rect.right - rect.left;
            m_height = rect.bottom - rect.top;
        }
    }
    
    HBITMAP Window::loadBMP(const std::wstring& _path) {
        return static_cast<HBITMAP>(LoadImageW(
            nullptr,
            _path.c_str(),
            IMAGE_BITMAP,
            0, 0,
            LR_LOADFROMFILE | LR_CREATEDIBSECTION
        ));
    }

    RECT Window::calculateFitRect(int _srcW, int _srcH, int _dstW, int _dstH) const {
        RECT rc{};
    
        float srcAspect = static_cast<float>(_srcW) / static_cast<float>(_srcH);
        float dstAspect = static_cast<float>(_dstW) / static_cast<float>(_dstH);
    
        if (srcAspect > dstAspect) {
            // Fit to height
            int fittedHeight = static_cast<int>(_dstW / srcAspect);
            rc.left = 0;
            rc.top = (_dstH - fittedHeight) / 2; // Center verticaly
            rc.right = _dstW;
            rc.bottom = rc.top + fittedHeight;
        } else {
            // Fit to width
            int fittedWidth = static_cast<int>(_dstH * srcAspect);
            rc.left = (_dstW - fittedWidth) / 2; // Center horizontally
            rc.top = 0;
            rc.right = rc.left + fittedWidth;
            rc.bottom = _dstH;
        }
    
        return rc;
    }

    // TO DO : This function was generated using ChatGPT, need to replace !
    HBITMAP Window::createCenteredBitmap(HBITMAP _source, int _width, int _height, uint32_t _bgColor) {
        BITMAP bm;
        if (!GetObject(_source, sizeof(bm), &bm)) return nullptr;

        // Extract RGB components (0x00RRGGBB format expected, or use 0x00000000 for black)
        COLORREF bgColor = RGB(
            (_bgColor >> 16) & 0xFF,  // Red
            (_bgColor >> 8) & 0xFF,   // Green
            _bgColor & 0xFF           // Blue
        );

        // Setup DCs
        HDC hdcScreen = GetDC(nullptr);
        HDC hdcSrc = CreateCompatibleDC(hdcScreen);
        HDC hdcDst = CreateCompatibleDC(hdcScreen);

        // Create destination bitmap (window-sized)
        HBITMAP hDst = CreateCompatibleBitmap(hdcScreen, _width, _height);
        if (!hDst) {
            DeleteDC(hdcSrc);
            DeleteDC(hdcDst);
            ReleaseDC(nullptr, hdcScreen);
            return nullptr;
        }

        // Select bitmaps into DCs
        SelectObject(hdcSrc, _source);
        SelectObject(hdcDst, hDst);

        // Fill entire background with letterbox color
        RECT fullRect = {0, 0, _width, _height};
        HBRUSH hFillBrush = CreateSolidBrush(bgColor);
        FillRect(hdcDst, &fullRect, hFillBrush);
        DeleteObject(hFillBrush);

        // Calculate centered rect preserving aspect ratio
        RECT fitRect = calculateFitRect(bm.bmWidth, bm.bmHeight, _width, _height);

        // Stretch image into centered position with quality scaling
        SetStretchBltMode(hdcDst, HALFTONE);
        StretchBlt(hdcDst,
                   fitRect.left, fitRect.top,
                   fitRect.right - fitRect.left,
                   fitRect.bottom - fitRect.top,
                   hdcSrc, 0, 0, bm.bmWidth, bm.bmHeight,
                   SRCCOPY);

        // Cleanup
        DeleteDC(hdcSrc);
        DeleteDC(hdcDst);
        ReleaseDC(nullptr, hdcScreen);

        return hDst;
    }

    void Window::pollEvents() {
        computeWindowRect();

        while (PeekMessage(&m_msg, nullptr, 0, 0, PM_REMOVE)) {
            if (m_msg.message == WM_QUIT) {
                m_opened = false;
            }

            TranslateMessage(&m_msg);
            DispatchMessage(&m_msg);
        }
    }

    int Window::getX() const {
        return m_x;
    }
    
    int Window::getY() const {
        return m_y;
    }
    
    uint Window::getWidth() const {
        return m_width;
    }
    
    uint Window::getHeight() const {
        return m_height;
    }

    HWND Window::getNativeHandle() const {
        return m_handle;
    }

    void Window::setFullscreen(bool _fullscreen) {
        if (m_fullscreen == _fullscreen) return;

        m_fullscreen = _fullscreen;

        if (m_fullscreen) {
            // Save current window state
            m_windowedStyle = GetWindowLong(m_handle, GWL_STYLE);
            RECT windowRect;
            GetWindowRect(m_handle, &windowRect);
            m_windowedX = windowRect.left;
            m_windowedY = windowRect.top;
            m_windowedWidth = windowRect.right - windowRect.left;
            m_windowedHeight = windowRect.bottom - windowRect.top;

            // Get monitor info
            HMONITOR hMonitor = MonitorFromWindow(m_handle, MONITOR_DEFAULTTONEAREST);
            MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
            GetMonitorInfo(hMonitor, &monitorInfo);

            // Set borderless style
            SetWindowLong(m_handle, GWL_STYLE, 
                WS_POPUP | WS_VISIBLE);
            
            SetWindowPos(m_handle, HWND_TOP,
                monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.top,
                monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                SWP_FRAMECHANGED | SWP_NOZORDER);

        } else {
            // Restore window state
            SetWindowLong(m_handle, GWL_STYLE, 
                m_windowedStyle | WS_VISIBLE);
            
            SetWindowPos(m_handle, HWND_NOTOPMOST,
                m_windowedX, m_windowedY,
                m_windowedWidth, m_windowedHeight,
                SWP_FRAMECHANGED | SWP_NOZORDER);
        }

        computeWindowRect();
    }

    void Window::toggleFullscreen() {
        setFullscreen(!m_fullscreen);
    }

    bool Window::isFullscreen() const {
        return m_fullscreen;
    }

    LRESULT Window::windowProc(HWND _hwnd, UINT _message, WPARAM _wParam, LPARAM _lParam) {
        switch (_message) {
            case WM_CLOSE:
                m_opened = false;
                DestroyWindow(_hwnd);
                PostQuitMessage(0);
                return 0;
                
            case WM_SIZE:
                // Update dimensions on resize
                if (!m_fullscreen && _wParam != SIZE_MINIMIZED) {
                    computeWindowRect();
                }
                return 0;

            default:
                return DefWindowProc(_hwnd, _message, _wParam, _lParam);
        }
    }

    LRESULT CALLBACK Window::procInitializer(HWND _hwnd, UINT _message, WPARAM _wParam, LPARAM _lParam) {
        Window* window = nullptr;
        
        if (_message == WM_NCCREATE) {
            auto* cs = reinterpret_cast<CREATESTRUCT*>(_lParam);
            window = static_cast<Window*>(cs->lpCreateParams);
            
            SetLastError(0);
            if (SetWindowLongPtr(_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window)) == 0 
                && GetLastError() != 0) {
                return FALSE;
            }
        } else {
            window = reinterpret_cast<Window*>(GetWindowLongPtr(_hwnd, GWLP_USERDATA));
        }
        
        if (window) {
            return window->windowProc(_hwnd, _message, _wParam, _lParam);
        }
        
        return DefWindowProc(_hwnd, _message, _wParam, _lParam);
    }
}