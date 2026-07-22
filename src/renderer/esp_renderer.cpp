#define NOMINMAX
#include "esp_renderer.h"
#include "../imgui/imgui.h"
#include <cmath>
#include <algorithm>

ESPRenderer::ESPRenderer() {
    OutputDebugStringA("[ESPRenderer] Initialized\n");
}

RGB ESPRenderer::LerpColor(const RGB& a, const RGB& b, float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return RGB{
        static_cast<int>(a.R + (b.R - a.R) * t),
        static_cast<int>(a.G + (b.G - a.G) * t),
        static_cast<int>(a.B + (b.B - a.B) * t),
        255
    };
}

void ESPRenderer::DrawCornersBox(const Vector3& head_pos, const Vector3& feet_pos,
                                 const GameData& game_data, float width, float height,
                                 const RGB& color, float thickness) {
    float corner_size = width * 0.2f;
    float left = head_pos.x - (width / 2);
    float top = head_pos.y - 5;
    float right = left + width;
    float bottom = top + height + 5;

    auto drawList = ImGui::GetBackgroundDrawList();
    if (!drawList) return;

    ImU32 col = IM_COL32(color.R, color.G, color.B, color.A);

    // Top-left corner
    drawList->AddLine(ImVec2(left, top), ImVec2(left + corner_size, top), col, thickness);
    drawList->AddLine(ImVec2(left, top), ImVec2(left, top + corner_size), col, thickness);

    // Top-right corner
    drawList->AddLine(ImVec2(right, top), ImVec2(right - corner_size, top), col, thickness);
    drawList->AddLine(ImVec2(right, top), ImVec2(right, top + corner_size), col, thickness);

    // Bottom-left corner
    drawList->AddLine(ImVec2(left, bottom), ImVec2(left + corner_size, bottom), col, thickness);
    drawList->AddLine(ImVec2(left, bottom), ImVec2(left, bottom - corner_size), col, thickness);

    // Bottom-right corner
    drawList->AddLine(ImVec2(right, bottom), ImVec2(right - corner_size, bottom), col, thickness);
    drawList->AddLine(ImVec2(right, bottom), ImVec2(right, bottom - corner_size), col, thickness);
}

void ESPRenderer::DrawFilledBox(const Vector3& head_pos, const Vector3& feet_pos,
                                const GameData& game_data, const RGB& color) {
    float width = (feet_pos.y - head_pos.y) / 1.6f;
    float left = head_pos.x - (width / 2);
    float top = head_pos.y - 5;
    float height = feet_pos.y - head_pos.y + 5;

    render::DrawFilledRect(
        static_cast<int>(left),
        static_cast<int>(top),
        static_cast<int>(width),
        static_cast<int>(height),
        color
    );
}

void ESPRenderer::DrawGradientBox(const EntityData& entity, const GameData& game_data,
                                  float width, float height, const Vector3& head_pos) {
    Vector3 feet_pos = entity.origin.w2s(game_data.view_matrix);
    float left = head_pos.x - (width / 2);
    float top = head_pos.y - 5;
    float bar_height = height / 100.0f;

    RGB healthGreen{0, 255, 0, 255};
    RGB healthRed{255, 0, 0, 255};

    for (int i = 0; i < entity.health && i < 100; i++) {
        float percent = i / 100.0f;
        RGB color = LerpColor(healthRed, healthGreen, percent);

        render::DrawFilledRect(
            static_cast<int>(left),
            static_cast<int>(top + (height * (100 - i) / 100.0f)),
            static_cast<int>(width),
            static_cast<int>(bar_height),
            color
        );
    }
}

void ESPRenderer::Draw3DBox(const EntityData& entity, const GameData& game_data,
                            const RGB& color, float thickness) {
    Vector3 screenHead = entity.head.w2s(game_data.view_matrix);
    Vector3 screenFeet = entity.origin.w2s(game_data.view_matrix);

    float width = (screenFeet.y - screenHead.y) / 1.6f;
    float height = screenFeet.y - screenHead.y;

    float left = screenHead.x - (width / 2);
    float right = left + width;
    float top = screenHead.y - 5;
    float bottom = top + height + 5;

    // Draw perspective box
    auto drawList = ImGui::GetBackgroundDrawList();
    if (!drawList) return;

    ImU32 col = IM_COL32(color.R, color.G, color.B, color.A);

    // Front face
    drawList->AddRect(ImVec2(left, top), ImVec2(right, bottom), col, 0.0f, 0, thickness);

    // Depth perspective lines
    float depth_offset = width * 0.1f;
    drawList->AddLine(ImVec2(left, top), ImVec2(left + depth_offset, top - depth_offset), col, thickness);
    drawList->AddLine(ImVec2(right, top), ImVec2(right + depth_offset, top - depth_offset), col, thickness);
    drawList->AddLine(ImVec2(left, bottom), ImVec2(left + depth_offset, bottom - depth_offset), col, thickness);
    drawList->AddLine(ImVec2(right, bottom), ImVec2(right + depth_offset, bottom - depth_offset), col, thickness);
}

void ESPRenderer::DrawBoxStyle(const Vector3& head_pos, const Vector3& feet_pos,
                               const GameData& game_data, const EntityData& entity,
                               BoxStyle style, const RGB& color, float thickness) {
    Vector3 screenFeetPos = feet_pos.w2s(game_data.view_matrix);
    Vector3 screenHeadPos = head_pos.w2s(game_data.view_matrix);

    if (screenHeadPos.z < 0.01f || screenFeetPos.z < 0.01f) return;

    float width = (screenFeetPos.y - screenHeadPos.y) / 1.6f;
    float height = screenFeetPos.y - screenHeadPos.y;

    switch (style) {
    case BoxStyle::CORNERS:
        DrawCornersBox(screenHeadPos, screenFeetPos, game_data, width, height, color, thickness);
        break;
    case BoxStyle::FILLED:
        DrawFilledBox(screenHeadPos, screenFeetPos, game_data, color);
        break;
    case BoxStyle::OUTLINED:
        render::DrawRect(
            static_cast<int>(screenHeadPos.x - (width / 2)),
            static_cast<int>(screenHeadPos.y - 5),
            static_cast<int>(width),
            static_cast<int>(height + 5),
            color, thickness
        );
        break;
    case BoxStyle::GRADIENT:
        DrawGradientBox(entity, game_data, width, height, screenHeadPos);
        break;
    case BoxStyle::THREE_D:
        Draw3DBox(entity, game_data, color, thickness);
        break;
    }
}
