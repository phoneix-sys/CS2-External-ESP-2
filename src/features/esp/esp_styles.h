#pragma once
#include "../../math/vector.h"
#include "../../memory/memory.h"

enum class BoxStyle {
    CORNERS,      // Only corners
    FILLED,       // Filled box
    OUTLINED,     // Simple outline
    GRADIENT,     // Health-based gradient
    THREE_D       // 3D perspective box
};

struct ESPSettings {
    bool enabled = true;
    BoxStyle box_style = BoxStyle::OUTLINED;
    float max_distance = 300.0f;
    bool draw_health_bar = true;
    bool draw_skeleton = false;
    bool draw_name = true;
    bool draw_distance = false;
    float line_thickness = 1.5f;
    
    struct RGB {
        int R, G, B, A;
    };
    
    RGB enemy_color = { 255, 0, 0, 255 };     // Red
    RGB ally_color = { 0, 255, 0, 255 };      // Green
    RGB health_bar_bg = { 0, 0, 0, 255 };     // Black
};
