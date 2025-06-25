/*****************************************************************
 * 
 *****************************************************************
 *
 *   Copyright(C) 2024      Twin Oaks Computing, Inc
 *   All rights reserved.   Castle Rock, CO 80108
 *
 *****************************************************************/
#if defined(USE_DX11)

#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "robobrawlClock.hh"
#include <stdio.h>
#include <cstdint>
#include <d3d11.h>

extern bool roboClock::done;

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace backend {

  static WNDCLASSEXW              wc;
  static HWND                     hwnd = NULL;
  static ID3D11Device*            g_pd3dDevice = nullptr;
  static ID3D11DeviceContext*     g_pd3dDeviceContext = nullptr;
  static IDXGISwapChain*          g_pSwapChain = nullptr;
  static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
  static ID3D11RenderTargetView*  g_mainRenderTargetView = nullptr;
  
  bool CreateDeviceD3D(HWND hWnd);
  void CleanupDeviceD3D();
  void CreateRenderTarget();
  void CleanupRenderTarget();
  LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


  static HICON CreateIconFromBytes(HDC DC, int width, int height, uint8_t * bytes) {
    HICON hIcon = NULL;

    ICONINFO iconInfo = {
      TRUE, // fIcon, set to true if this is an icon, set to false if this is a cursor
      NULL, // xHotspot, set to null for icons
      NULL, // yHotspot, set to null for icons
      NULL, // Monochrome bitmap mask, set to null initially
      NULL  // Color bitmap mask, set to null initially
    };

    uint8_t * byte = bytes;
    uint32_t* rawBitmap = new uint32_t[width * height];
    ULONG uWidth = (ULONG)width;
    ULONG uHeight = (ULONG)height;
    uint32_t* bitmapPtr = rawBitmap;
    // input is RGBA, we need ARGB, so swap around...
    for (ULONG y = 0; y < uHeight; y++) {
      for (ULONG x = 0; x < uWidth; x++) {
        uint8_t R = (*byte++);
        uint8_t G = (*byte++);
        uint8_t B = (*byte++);
        uint8_t A = (*byte++);
        *bitmapPtr = (A<24) | (R<<16) | (G<<8) | B;
        bitmapPtr++;
      }
    }
    
    iconInfo.hbmColor = CreateBitmap(width, height, 1, 32, rawBitmap);
    if (iconInfo.hbmColor) {
      iconInfo.hbmMask = CreateCompatibleBitmap(DC, width, height);
      if (iconInfo.hbmMask) {
        hIcon = CreateIconIndirect(&iconInfo);
        if (hIcon == NULL) {
          fprintf(stderr, "Failed to create icon.");
        }
        DeleteObject(iconInfo.hbmMask);
      } else {
        fprintf(stderr, "Failed to create color mask.");
      }
      DeleteObject(iconInfo.hbmColor);
    } else {
      fprintf(stderr, "Failed to create bitmap mask.");
    }

    delete[] rawBitmap;

    return hIcon;
  }  

  // -----------------------------------------------------------------------------
  static void set_icon( uint8_t* data, int w, int h )
  {
    /*
      GLFWimage icon;
      icon.width = w;
      icon.height = h;
      icon.pixels = data;
      glfwSetWindowIcon( window, 1, &icon );
    */
    HDC hdcScreen = GetDC(NULL);
    HICON hIcon = CreateIconFromBytes( hdcScreen, w, h, data );
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
  }

  // -----------------------------------------------------------------------------
  static void
  init_backend( const char * main_window_title,
                int          width,
                int          height )
  {

    gui::dpi_scale = 0.5;

    const auto envDpiScale = getenv( "MD_DPI_SCALE" );
    if( envDpiScale )
      {
        const auto cnv = atof( envDpiScale );
        if( cnv != 0 )
          gui::dpi_scale = cnv;
      }
    width  = width  * gui::dpi_scale;
    height = height * gui::dpi_scale;
    
    // Create application window
    ImGui_ImplWin32_EnableDpiAwareness();
    wc =
      {
        sizeof(wc),
        CS_CLASSDC,
        WndProc,
        0L,
        0L,
        GetModuleHandle(nullptr),
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        L"ImGuiDashboard",
        nullptr
      };
  
    ::RegisterClassExW(&wc);

    RECT win_rect = {0, 0, width, height };
    
    AdjustWindowRect( &win_rect, WS_OVERLAPPEDWINDOW, false );

    hwnd =  ::CreateWindowW( wc.lpszClassName,
                             L"StackLight",
                             WS_OVERLAPPEDWINDOW,
                             100,
                             100,
                             win_rect.right - win_rect.left,
                             win_rect.bottom - win_rect.top,
                             nullptr,
                             nullptr,
                             wc.hInstance,
                             nullptr );
    gui::m_win_w = width;
    gui::m_win_h = height;

    SetWindowLong( hwnd, GWL_STYLE, GetWindowLong( hwnd, GWL_STYLE ) & ~WS_SIZEBOX );
    
    // Initialize Direct3D
    if ( !CreateDeviceD3D( hwnd ) )
      {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        hwnd = NULL;
      }
    else
      {
        // Show the window
        ::ShowWindow(hwnd, SW_SHOWDEFAULT);
        ::UpdateWindow(hwnd);

      }  
  }

  // -----------------------------------------------------------------------------
  static void
  init_renderer()
  {
    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init( hwnd );
    ImGui_ImplDX11_Init( g_pd3dDevice, g_pd3dDeviceContext );
  }


  // -----------------------------------------------------------------------------
  static void
  poll_events()
  {
    MSG msg;
    while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
      {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
        if (msg.message == WM_QUIT)
          roboClock::done = true;
      }

    // Handle window resize (we don't resize directly in the WM_SIZE handler)
    if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
      {
        CleanupRenderTarget();
        g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
        gui::m_win_w = g_ResizeWidth;
        gui::m_win_h = g_ResizeHeight;
        g_ResizeWidth = g_ResizeHeight = 0;
        CreateRenderTarget();
      }
  }

  // -----------------------------------------------------------------------------
  static void
  new_frame()
  {
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
  }


  // -----------------------------------------------------------------------------
  static void
  render()
  {
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    const float clear_color_with_alpha[4] = {
      clear_color.x * clear_color.w,
      clear_color.y * clear_color.w,
      clear_color.z * clear_color.w,
      clear_color.w };
        
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    g_pSwapChain->Present(1, 0); // Present with vsync
    //g_pSwapChain->Present(0, 0); // Present without vsync  
  }


  // -----------------------------------------------------------------------------
  static void
  close_renderer()
  {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
  }

  // -----------------------------------------------------------------------------
  static void
  close_backend( )
  {
    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);  
  }



  /* HELPERS:
   */
   

  bool CreateDeviceD3D(HWND hWnd)
  {
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr,
                                                D3D_DRIVER_TYPE_HARDWARE,
                                                nullptr,
                                                createDeviceFlags,
                                                featureLevelArray,
                                                2,
                                                D3D11_SDK_VERSION,
                                                &sd,
                                                &g_pSwapChain,
                                                &g_pd3dDevice,
                                                &featureLevel,
                                                &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
      res = D3D11CreateDeviceAndSwapChain(nullptr,
                                          D3D_DRIVER_TYPE_WARP,
                                          nullptr,
                                          createDeviceFlags,
                                          featureLevelArray,
                                          2,
                                          D3D11_SDK_VERSION,
                                          &sd,
                                          &g_pSwapChain,
                                          &g_pd3dDevice,
                                          &featureLevel,
                                          &g_pd3dDeviceContext);
    if (res != S_OK)
      return false;

    CreateRenderTarget();
    return true;
  }

  void CleanupDeviceD3D()
  {
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
  }

  void CreateRenderTarget()
  {
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
  }

  void CleanupRenderTarget()
  {
    if (g_mainRenderTargetView)
      {
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = nullptr;
      }
  }


  // Win32 message handler
  // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
  // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
  // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
  // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
  LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
  {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
      return true;

    switch (msg)
      {
      case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
          return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
        
      case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
          return 0;
        break;
        
      case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
      }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
  }
  
}

#endif
