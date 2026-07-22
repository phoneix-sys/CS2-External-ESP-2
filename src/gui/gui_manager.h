#pragma once
#include "../imgui/imgui.h"
#include "../features/esp/esp_styles.h"
#include "../features/aimbot/aimbot.h"
#include "../features/bhop/bhop.h"
#include "../features/glow/glow.h"
#include "../features/skeleton/skeleton.h"
#include "../core/entity_cache.h"
#include <memory>

class GUIManager {
public:
    GUIManager();
    ~GUIManager() = default;
    
    void Render();
    void Update();
    bool IsVisible() const { return visible; }
    void ToggleVisibility() { visible = !visible; }
    
    void SetPointers(ESP* esp, Aimbot* aim, BunnyHop* bhop, Glow* glow, SkeletonRenderer* skel) {
        esp_ptr = esp;
        aimbot_ptr = aim;
        bhop_ptr = bhop;
        glow_ptr = glow;
        skeleton_ptr = skel;
    }
    
private:
    bool visible = true;
    ImVec2 window_size = { 550, 700 };
    int active_tab = 0;
    
    // Feature pointers
    ESP* esp_ptr = nullptr;
    Aimbot* aimbot_ptr = nullptr;
    BunnyHop* bhop_ptr = nullptr;
    Glow* glow_ptr = nullptr;
    SkeletonRenderer* skeleton_ptr = nullptr;
    
    // Tab render functions
    void RenderESPTab();
    void RenderAimbotTab();
    void RenderBhopTab();
    void RenderGlowTab();
    void RenderSkeletonTab();
    void RenderSettingsTab();
    void RenderAboutTab();
    
    // Helper functions
    void ApplyStyle();
};
