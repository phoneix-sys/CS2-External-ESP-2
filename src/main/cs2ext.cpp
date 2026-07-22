#define NOMINMAX
#include <Windows.h>
#include <TlHelp32.h>
#include <d3d11.h>
#include <dxgi.h>
#include <chrono>
#include <thread>
#include <string>
#include <memory>

#include "src/math/vector.h"
#include "src/memory/memory.h"
#include "src/offsets/offsets.h"
#include "src/imgui/imgui.h"
#include "src/imgui/imgui_impl_dx11.h"
#include "src/imgui/imgui_impl_win32.h"
#include "src/core/entity_cache.h"
#include "src/renderer/renderer.h"
#include "src/renderer/esp_renderer.h"
#include "src/gui/gui_manager.h"
#include "src/features/aimbot/aimbot.h"
#include "src/features/bhop/bhop.h"
#include "src/features/glow/glow.h"
#include "src/features/skeleton/skeleton.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dwmapi.lib")

typedef struct tagMARGINS {
    int cxLeftWidth;
    int cxRightWidth;
    int cyTopHeight;
    int cyBottomHeight;
} MARGINS, * PMARGINS;

extern "C" HRESULT WINAPI DwmExtendFrameIntoClientArea(HWND hWnd, const MARGINS* pMarInset);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static int currentFPS = 0;
static int frameCount = 0;
static auto lastFPSUpdate = std::chrono::high_resolution_clock::now();

static constexpr int MAX_TARGET_FPS = 1000;
static constexpr int MIN_DATA_UPDATE_MS = 1;
static constexpr int MAX_DATA_UPDATE_MS = 6;

LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
    if (ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param)) {
        return 0L;
    }

    switch (message) {
    case WM_DESTROY: {
        PostQuitMessage(0);
        return 0L;
    }
    case WM_SIZE: {
        if (w_param == SIZE_MINIMIZED)
            return 0L;
        return 0L;
    }
    case WM_SYSCOMMAND: {
        if ((w_param & 0xfff0) == SC_KEYMENU)
            return 0L;
        break;
    }
    case WM_KEYDOWN: {
        if (w_param == VK_INSERT) {
            // Toggle GUI visibility
            // This would be handled by GUIManager
        }
        break;
    }
    }
    return DefWindowProc(window, message, w_param, l_param);
}

void GetNativeRefreshRate(HWND hwnd, UINT& numerator, UINT& denominator) {
    DXGI_MODE_DESC modeDesc = {};
    IDXGIFactory* pFactory = nullptr;
    IDXGIAdapter* pAdapter = nullptr;
    IDXGIOutput* pOutput = nullptr;

    if (SUCCEEDED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory))) {
        if (SUCCEEDED(pFactory->EnumAdapters(0, &pAdapter))) {
            if (SUCCEEDED(pAdapter->EnumOutputs(0, &pOutput))) {
                UINT numModes = 0;
                pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &numModes, nullptr);

                if (numModes > 0) {
                    DXGI_MODE_DESC* modes = new DXGI_MODE_DESC[numModes];
                    if (SUCCEEDED(pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &numModes, modes))) {
                        for (UINT i = 0; i < numModes; i++) {
                            if (modes[i].Width == GetSystemMetrics(SM_CXSCREEN) &&
                                modes[i].Height == GetSystemMetrics(SM_CYSCREEN)) {
                                numerator = modes[i].RefreshRate.Numerator;
                                denominator = modes[i].RefreshRate.Denominator;
                                break;
                            }
                        }
                    }
                    delete[] modes;
                }
                pOutput->Release();
            }
            pAdapter->Release();
        }
        pFactory->Release();
    }

    if (numerator == 0 || denominator == 0) {
        numerator = 60;
        denominator = 1;
    }
}

void CleanupResources(
    IDXGISwapChain*& swap_chain,
    ID3D11Device*& device,
    ID3D11DeviceContext*& device_context,
    ID3D11RenderTargetView*& render_target_view,
    HWND window,
    WNDCLASSEXW& wc
) {
    timeEndPeriod(1);
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (swap_chain) swap_chain->Release();
    if (device_context) device_context->Release();
    if (device) device->Release();
    if (render_target_view) render_target_view->Release();

    DestroyWindow(window);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

class CheatEngine {
private:
    std::unique_ptr<ESP> esp;
    std::unique_ptr<Aimbot> aimbot;
    std::unique_ptr<BunnyHop> bhop;
    std::unique_ptr<Glow> glow;
    std::unique_ptr<SkeletonRenderer> skeleton;
    std::unique_ptr<GUIManager> gui;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<ESPRenderer> esp_renderer;
    std::unique_ptr<EntityCache> entity_cache;
    GameData game_data;

public:
    CheatEngine() {
        esp = std::make_unique<ESP>();
        aimbot = std::make_unique<Aimbot>();
        bhop = std::make_unique<BunnyHop>();
        glow = std::make_unique<Glow>();
        skeleton = std::make_unique<SkeletonRenderer>();
        gui = std::make_unique<GUIManager>();
        renderer = std::make_unique<Renderer>();
        esp_renderer = std::make_unique<ESPRenderer>();
        entity_cache = std::make_unique<EntityCache>(144, 1, 128);

        // Set pointers for GUI
        gui->SetPointers(esp.get(), aimbot.get(), bhop.get(), glow.get(), skeleton.get());
    }

    bool Initialize(DWORD pid, uintptr_t client) {
        game_data.pid = pid;
        game_data.client = client;
        return game_data.IsValid();
    }

    void Update() {
        // Update game data
        {
            std::lock_guard<std::mutex> lock(game_data.mtx);
            game_data.localPlayerPawn = mem::ReadMem<std::uintptr_t>(game_data.client + dwLocalPlayerPawn);
            game_data.entList = mem::ReadMem<std::uintptr_t>(game_data.client + dwEntityList);
            game_data.view_matrix = mem::ReadMem<view_matrix_t>(game_data.client + dwViewMatrix);
            if (game_data.localPlayerPawn) {
                game_data.localTeam = mem::ReadMem<int>(game_data.localPlayerPawn + m_iTeamNum);
            }
        }

        // Get cached entities
        const auto& entities = entity_cache->GetEntities(game_data.client, game_data);

        // Update features
        aimbot->Update(game_data, entities);
        bhop->Update(game_data.localPlayerPawn, game_data.client);
        glow->Update(game_data.client, game_data, entities);
    }

    void RenderFrame(const std::vector<EntityData>& entities) {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Render overlay features
        render::DrawESP(entities, game_data);

        // Render GUI
        gui->Render();

        ImGui::Render();
    }

    void ProcessRenderQueue() {
        renderer->ProcessRenderQueue();
    }
};

INT APIENTRY WinMain(HINSTANCE instance, HINSTANCE, PSTR, INT cmd_show) {
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = window_procedure;
    wc.hInstance = instance;
    wc.lpszClassName = L"CS2EXTERNAL";

    if (!RegisterClassExW(&wc)) {
        MessageBoxW(NULL, L"Window Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    const HWND window = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_NOACTIVATE,
        wc.lpszClassName,
        L"CS2 External ESP v2.0",
        WS_POPUP,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        nullptr, nullptr, wc.hInstance, nullptr
    );

    if (!window) {
        MessageBoxW(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

    RECT client_area{}, window_area{};
    GetClientRect(window, &client_area);
    GetWindowRect(window, &window_area);
    POINT diff{};
    ClientToScreen(window, &diff);
    const MARGINS margins{
        window_area.left + (diff.x - window_area.left),
        window_area.top + (diff.y - window_area.top),
        client_area.right,
        client_area.bottom
    };
    DwmExtendFrameIntoClientArea(window, &margins);

    DXGI_SWAP_CHAIN_DESC sd{};
    UINT refreshRateNumerator = 0, refreshRateDenominator = 0;
    GetNativeRefreshRate(window, refreshRateNumerator, refreshRateDenominator);

    double monitor_hz = (refreshRateDenominator != 0) ?
        static_cast<double>(refreshRateNumerator) / refreshRateDenominator : 60.0;

    sd.BufferDesc.RefreshRate.Numerator = refreshRateNumerator;
    sd.BufferDesc.RefreshRate.Denominator = refreshRateDenominator;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.SampleDesc.Count = 1U;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 2U;
    sd.OutputWindow = window;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    constexpr D3D_FEATURE_LEVEL levels[2]{ D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* device_context = nullptr;
    IDXGISwapChain* swap_chain = nullptr;
    ID3D11RenderTargetView* render_target_view = nullptr;
    D3D_FEATURE_LEVEL level;

    if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0U, levels, 2U, D3D11_SDK_VERSION, &sd, &swap_chain, &device, &level, &device_context))) {
        MessageBoxW(NULL, L"DirectX Initialization Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        DestroyWindow(window);
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 0;
    }

    ID3D11Texture2D* back_buffer = nullptr;
    if (FAILED(swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer)))) {
        MessageBoxW(NULL, L"Failed to get back buffer!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        swap_chain->Release();
        device->Release();
        device_context->Release();
        DestroyWindow(window);
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 0;
    }

    if (FAILED(device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view))) {
        MessageBoxW(NULL, L"Failed to create render target view!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        back_buffer->Release();
        swap_chain->Release();
        device->Release();
        device_context->Release();
        DestroyWindow(window);
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 0;
    }
    back_buffer->Release();

    ShowWindow(window, cmd_show);
    UpdateWindow(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.IniFilename = nullptr;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(device, device_context);

    const DWORD pid = mem::GetProcID(L"cs2.exe");
    if (pid == 0) {
        MessageBoxW(NULL, L"Failed to find CS2 process!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        CleanupResources(swap_chain, device, device_context, render_target_view, window, wc);
        return 1;
    }

    const std::uintptr_t client = mem::GetModuleBase(pid, L"client.dll");
    if (client == 0) {
        MessageBoxW(NULL, L"Failed to find client.dll!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        CleanupResources(swap_chain, device, device_context, render_target_view, window, wc);
        return 1;
    }

    timeBeginPeriod(1);
    SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);

    // Initialize cheat engine
    CheatEngine engine;
    if (!engine.Initialize(pid, client)) {
        MessageBoxW(NULL, L"Failed to initialize cheat engine!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        CleanupResources(swap_chain, device, device_context, render_target_view, window, wc);
        return 1;
    }

    const double targetFrameTimeMs = 1000.0 / MAX_TARGET_FPS;
    bool running = true;

    while (running) {
        auto frameStart = std::chrono::high_resolution_clock::now();

        MSG msg;
        while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                running = false;
            ImGui_ImplWin32_WndProcHandler(window, msg.message, msg.wParam, msg.lParam);
        }

        if (!running) break;

        // Update
        engine.Update();

        // Render
        frameCount++;
        auto fpsElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(frameStart - lastFPSUpdate).count();
        if (fpsElapsed >= 1000) {
            currentFPS = frameCount;
            frameCount = 0;
            lastFPSUpdate = frameStart;
        }

        // Prepare render
        GameData dummy_data; // Replace with actual game_data from engine
        std::vector<EntityData> dummy_entities; // Replace with actual entities
        engine.RenderFrame(dummy_entities);
        engine.ProcessRenderQueue();

        // Present
        constexpr float clear_color[4]{ 0.f, 0.f, 0.f, 0.f };
        device_context->OMSetRenderTargets(1U, &render_target_view, nullptr);
        device_context->ClearRenderTargetView(render_target_view, clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        swap_chain->Present(0, 0);

        auto frameEnd = std::chrono::high_resolution_clock::now();
        auto frameTimeMs = std::chrono::duration_cast<std::chrono::microseconds>(frameEnd - frameStart).count() / 1000.0;

        if (frameTimeMs < targetFrameTimeMs) {
            int sleepTimeUs = static_cast<int>((targetFrameTimeMs - frameTimeMs) * 1000);
            if (sleepTimeUs > 0 && sleepTimeUs < 1000) {
                std::this_thread::sleep_for(std::chrono::microseconds(sleepTimeUs));
            }
        }
    }

    CleanupResources(swap_chain, device, device_context, render_target_view, window, wc);
    return 0;
}
