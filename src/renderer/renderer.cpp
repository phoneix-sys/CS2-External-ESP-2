#define NOMINMAX
#include "renderer.h"
#include "../imgui/imgui.h"
#include "../core/entity_cache.h"
#include <algorithm>

Renderer::Renderer() {
    OutputDebugStringA("[Renderer] Initialized\n");
}

Renderer::~Renderer() {
    ClearQueue();
}

void Renderer::QueueRender(std::function<void()> task) {
    if (!task) return;
    std::lock_guard<std::mutex> lock(queue_mtx);
    render_queue.push_back(task);
}

void Renderer::ProcessRenderQueue() {
    std::lock_guard<std::mutex> lock(queue_mtx);
    for (auto& task : render_queue) {
        if (task) task();
    }
    render_queue.clear();
}

void Renderer::ClearQueue() {
    std::lock_guard<std::mutex> lock(queue_mtx);
    render_queue.clear();
}

namespace render {
    void DrawRect(int x, int y, int w, int h, RGB color, float thickness) {
        auto drawList = ImGui::GetBackgroundDrawList();
        if (drawList) {
            drawList->AddRect(
                ImVec2(static_cast<float>(x), static_cast<float>(y)),
                ImVec2(static_cast<float>(x + w), static_cast<float>(y + h)),
                IM_COL32(color.R, color.G, color.B, color.A),
                0.0f, 0, thickness
            );
        }
    }

    void DrawFilledRect(int x, int y, int w, int h, RGB color) {
        auto drawList = ImGui::GetBackgroundDrawList();
        if (drawList) {
            drawList->AddRectFilled(
                ImVec2(static_cast<float>(x), static_cast<float>(y)),
                ImVec2(static_cast<float>(x + w), static_cast<float>(y + h)),
                IM_COL32(color.R, color.G, color.B, color.A)
            );
        }
    }

    void DrawLine(float x1, float y1, float x2, float y2, RGB color, float thickness) {
        auto drawList = ImGui::GetBackgroundDrawList();
        if (drawList) {
            drawList->AddLine(
                ImVec2(x1, y1),
                ImVec2(x2, y2),
                IM_COL32(color.R, color.G, color.B, color.A),
                thickness
            );
        }
    }

    void DrawText(int x, int y, RGB color, const char* text) {
        auto drawList = ImGui::GetBackgroundDrawList();
        if (drawList) {
            drawList->AddText(
                ImVec2(static_cast<float>(x), static_cast<float>(y)),
                IM_COL32(color.R, color.G, color.B, color.A),
                text
            );
        }
    }

    void DrawCircle(int x, int y, int radius, RGB color, float thickness, int segments) {
        auto drawList = ImGui::GetBackgroundDrawList();
        if (drawList) {
            drawList->AddCircle(
                ImVec2(static_cast<float>(x), static_cast<float>(y)),
                static_cast<float>(radius),
                IM_COL32(color.R, color.G, color.B, color.A),
                segments, thickness
            );
        }
    }

    void DrawFilledCircle(int x, int y, int radius, RGB color, int segments) {
        auto drawList = ImGui::GetBackgroundDrawList();
        if (drawList) {
            drawList->AddCircleFilled(
                ImVec2(static_cast<float>(x), static_cast<float>(y)),
                static_cast<float>(radius),
                IM_COL32(color.R, color.G, color.B, color.A),
                segments
            );
        }
    }

    void DrawESP(const std::vector<EntityData>& entities, const GameData& game_data) {
        auto drawList = ImGui::GetBackgroundDrawList();
        if (!drawList) return;

        for (const auto& entity : entities) {
            if (!entity.valid) continue;

            Vector3 headPos = entity.hasBoneData ? entity.headBone : entity.head;
            Vector3 screenFeetPos = entity.origin.w2s(game_data.view_matrix);
            Vector3 screenHeadPos = headPos.w2s(game_data.view_matrix);

            if (screenHeadPos.z < 0.01f || screenFeetPos.z < 0.01f) continue;

            float boxHeight = screenFeetPos.y - screenHeadPos.y;
            if (boxHeight < 5.0f) continue;

            float boxWidth = boxHeight / 1.6f;
            float left = screenHeadPos.x - (boxWidth / 2);
            float top = screenHeadPos.y - 5;

            RGB boxColor = (entity.team == game_data.localTeam) ? 
                RGB{0, 255, 0, 255} : RGB{255, 0, 0, 255};

            DrawRect(
                static_cast<int>(left),
                static_cast<int>(top),
                static_cast<int>(boxWidth),
                static_cast<int>(boxHeight + 5),
                boxColor, 1.5f
            );

            // Health bar
            int healthIndex = std::clamp(entity.health, 0, 100);
            RGB healthColor{255 - (healthIndex * 255 / 100), healthIndex * 255 / 100, 0, 255};

            DrawFilledRect(
                static_cast<int>(left - 6),
                static_cast<int>(top),
                2,
                static_cast<int>(boxHeight + 5),
                RGB{0, 0, 0, 255}
            );

            float barHeight = (boxHeight + 5) * (entity.health / 100.0f);
            DrawFilledRect(
                static_cast<int>(left - 6),
                static_cast<int>(top + (boxHeight + 5 - barHeight)),
                2,
                static_cast<int>(barHeight),
                healthColor
            );
        }
    }

    void DrawDebugInfo(const GameData& game_data, size_t entity_count) {
        char buffer[128];
        snprintf(buffer, sizeof(buffer),
            "P:0x%p E:%zu T:%d",
            (void*)game_data.localPlayerPawn,
            entity_count,
            game_data.localTeam
        );

        RGB white{255, 255, 255, 255};
        DrawText(10, 10, white, buffer);
    }
}
