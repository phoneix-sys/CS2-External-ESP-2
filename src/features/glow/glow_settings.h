#pragma once
#include "../../math/vector.h"

struct RGB {
    int R, G, B, A;
};

struct GlowSettings {
    bool enabled = false;
    float intensity = 1.0f;                           // 0.1 to 5.0
    RGB enemy_color = { 255, 0, 0, 255 };           // Red
    RGB ally_color = { 0, 255, 0, 255 };            // Green
    RGB visible_color = { 255, 255, 0, 255 };       // Yellow for visible
    bool visualized_only = false;                   // Only glow if on screen
    bool through_walls = true;                      // Glow through walls
    float line_width = 2.0f;                        // Glow line width
};
