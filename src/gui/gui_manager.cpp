#define NOMINMAX
#include "gui_manager.h"
#include "../features/esp/esp.h"

GUIManager::GUIManager() {
    ApplyStyle();
    OutputDebugStringA("[GUIManager] Initialized\n");
}

void GUIManager::ApplyStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.Alpha = 0.95f;
    
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.95f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.85f);
    colors[ImGuiCol_Button] = ImVec4(0.3f, 0.3f, 0.3f, 0.8f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.4f, 0.4f, 0.4f, 0.9f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
}

void GUIManager::Update() {
    // Update logic if needed
}

void GUIManager::Render() {
    if (!visible) return;
    
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(window_size, ImGuiCond_FirstUseEver);
    
    if (ImGui::Begin("CS2 External Cheat##Main", &visible, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("[+] CS2 ESP | Aimbot | Bhop | Glow | Skeleton");
        ImGui::Separator();
        
        if (ImGui::BeginTabBar("##MainTabs", ImGuiTabBarFlags_None)) {
            if (ImGui::BeginTabItem("ESP##tab")) {
                RenderESPTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Aimbot##tab")) {
                RenderAimbotTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Bhop##tab")) {
                RenderBhopTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Glow##tab")) {
                RenderGlowTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Skeleton##tab")) {
                RenderSkeletonTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Settings##tab")) {
                RenderSettingsTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("About##tab")) {
                RenderAboutTab();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}

void GUIManager::RenderESPTab() {
    ImGui::Text("ESP Settings");
    ImGui::Separator();
    
    static bool esp_enabled = true;
    static float esp_distance = 300.0f;
    static int box_style = 0;
    static bool draw_health = true;
    static bool draw_skeleton = false;
    static bool draw_name = true;
    static bool draw_distance = false;
    
    ImGui::Checkbox("Enable ESP##esp_enabled", &esp_enabled);
    ImGui::SliderFloat("Max Distance##ESP", &esp_distance, 100, 2000);
    
    const char* box_styles[] = { "Corners", "Filled", "Outlined", "Gradient", "3D" };
    ImGui::Combo("Box Style##esp_box", &box_style, box_styles, 5);
    
    ImGui::Checkbox("Draw Health Bar##esp_health", &draw_health);
    ImGui::Checkbox("Draw Skeleton##esp_skel", &draw_skeleton);
    ImGui::Checkbox("Draw Name##esp_name", &draw_name);
    ImGui::Checkbox("Draw Distance##esp_dist", &draw_distance);
    
    ImGui::Separator();
    static float enemy_color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    static float ally_color[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
    
    ImGui::ColorEdit4("Enemy Color##esp", enemy_color);
    ImGui::ColorEdit4("Ally Color##esp", ally_color);
}

void GUIManager::RenderAimbotTab() {
    ImGui::Text("Aimbot Settings");
    ImGui::Separator();
    
    if (!aimbot_ptr) {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Aimbot not initialized");
        return;
    }
    
    static bool aimbot_enabled = false;
    static int aim_mode = 0;
    static float smoothness = 0.5f;
    static float fov = 20.0f;
    static bool prediction = false;
    static bool visible_only = true;
    
    ImGui::Checkbox("Enable Aimbot##aim_enabled", &aimbot_enabled);
    if (aimbot_enabled) aimbot_ptr->SetMode(AimMode::SMOOTH);
    
    const char* aim_modes[] = { "Smooth", "Instant", "Prediction" };
    ImGui::Combo("Aim Mode##aim", &aim_mode, aim_modes, 3);
    
    ImGui::SliderFloat("Smoothness##Aim", &smoothness, 0.1f, 1.0f);
    aimbot_ptr->SetSmoothness(smoothness);
    
    ImGui::SliderFloat("FOV##Aim", &fov, 5.0f, 180.0f);
    aimbot_ptr->SetFOV(fov);
    
    ImGui::Checkbox("Prediction##aim", &prediction);
    aimbot_ptr->SetPrediction(prediction);
    
    ImGui::Checkbox("Visible Only##aim", &visible_only);
    
    ImGui::Separator();
    ImGui::TextDisabled("Lock Key: RMB");
    ImGui::TextDisabled("Disable Key: INSERT");
}

void GUIManager::RenderBhopTab() {
    ImGui::Text("Bhop Settings");
    ImGui::Separator();
    
    if (!bhop_ptr) {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Bhop not initialized");
        return;
    }
    
    static bool bhop_enabled = false;
    static bool auto_strafe = true;
    static float jump_height = 65.0f;
    
    ImGui::Checkbox("Enable Bhop##bhop_enabled", &bhop_enabled);
    bhop_ptr->SetEnabled(bhop_enabled);
    
    ImGui::Checkbox("Auto Strafe##bhop", &auto_strafe);
    bhop_ptr->SetAutoStrafe(auto_strafe);
    
    ImGui::SliderFloat("Jump Height##bhop", &jump_height, 32.0f, 150.0f);
    bhop_ptr->SetJumpHeight(jump_height);
    
    ImGui::Separator();
    ImGui::TextDisabled("Spacebar to jump");
}

void GUIManager::RenderGlowTab() {
    ImGui::Text("Glow Settings");
    ImGui::Separator();
    
    if (!glow_ptr) {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Glow not initialized");
        return;
    }
    
    static bool glow_enabled = false;
    static float glow_intensity = 1.0f;
    static bool glow_visible_only = false;
    static bool glow_through_walls = true;
    
    ImGui::Checkbox("Enable Glow##glow_enabled", &glow_enabled);
    glow_ptr->SetEnabled(glow_enabled);
    
    ImGui::SliderFloat("Glow Intensity##glow", &glow_intensity, 0.1f, 5.0f);
    glow_ptr->SetIntensity(glow_intensity);
    
    ImGui::Checkbox("Visible Only##glow", &glow_visible_only);
    ImGui::Checkbox("Through Walls##glow", &glow_through_walls);
    
    ImGui::Separator();
    static float enemy_glow[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    static float ally_glow[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
    
    ImGui::ColorEdit4("Enemy Glow##glow", enemy_glow);
    ImGui::ColorEdit4("Ally Glow##glow", ally_glow);
}

void GUIManager::RenderSkeletonTab() {
    ImGui::Text("Skeleton Settings");
    ImGui::Separator();
    
    if (!skeleton_ptr) {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Skeleton not initialized");
        return;
    }
    
    static bool skeleton_enabled = true;
    static float line_thickness = 1.5f;
    static bool draw_joints = true;
    
    ImGui::Checkbox("Enable Skeleton##skel_enabled", &skeleton_enabled);
    skeleton_ptr->SetEnabled(skeleton_enabled);
    
    ImGui::SliderFloat("Line Thickness##skel", &line_thickness, 0.5f, 3.0f);
    skeleton_ptr->SetLineThickness(line_thickness);
    
    ImGui::Checkbox("Draw Joints##skel", &draw_joints);
    
    ImGui::Separator();
    static float skeleton_color[4] = { 0.0f, 1.0f, 1.0f, 1.0f };
    ImGui::ColorEdit4("Skeleton Color##skel", skeleton_color);
}

void GUIManager::RenderSettingsTab() {
    ImGui::Text("General Settings");
    ImGui::Separator();
    
    static int fps_limit = 120;
    ImGui::SliderInt("FPS Limit##general", &fps_limit, 30, 360);
    
    static bool show_debug = false;
    ImGui::Checkbox("Show Debug Info##general", &show_debug);
    
    ImGui::Separator();
    ImGui::TextDisabled("Press INSERT to toggle GUI");
    
    if (ImGui::Button("Save Config##settings", ImVec2(100, 0))) {
        // Save config
    }
    ImGui::SameLine();
    if (ImGui::Button("Load Config##settings", ImVec2(100, 0))) {
        // Load config
    }
}

void GUIManager::RenderAboutTab() {
    ImGui::Text("CS2 External Cheat v2.0");
    ImGui::Separator();
    ImGui::Text("Features:");
    ImGui::BulletText("ESP Box Rendering (5 styles)");
    ImGui::BulletText("Aimbot (Smooth/Instant/Prediction)");
    ImGui::BulletText("Bunny Hop");
    ImGui::BulletText("Glow Effects");
    ImGui::BulletText("Skeleton Rendering");
    ImGui::BulletText("Health Bar");
    
    ImGui::Separator();
    ImGui::Text("Credits:");
    ImGui::TextDisabled("Original: phoneixdevv");
    ImGui::TextDisabled("Enhanced: Community Contributors");
    ImGui::TextDisabled("Offsets: a2x/cs2-dumper");
    
    ImGui::Separator();
    ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "WARNING: Use at your own risk!");
    ImGui::TextDisabled("This is for educational purposes only.");
}
