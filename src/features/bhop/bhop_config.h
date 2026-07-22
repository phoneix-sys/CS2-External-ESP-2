#pragma once

struct BhopConfig {
    bool enabled = false;
    bool auto_strafe = true;
    float jump_height = 65.0f;      // Default CS2 jump height
    float air_acceleration = 200.0f; // Air acceleration multiplier
    int jump_delay_ms = 50;          // Delay between jumps
    bool perfect_timing = true;      // Try to jump at perfect height
};
