#pragma once
#include "../../math/vector.h"

enum class AimMode {
    DISABLED,
    SMOOTH,
    INSTANT,
    PREDICTION
};

struct AimSettings {
    AimMode mode = AimMode::SMOOTH;
    float smoothness = 0.5f;          // 0.1 to 1.0
    float fov = 20.0f;                // Field of view in degrees
    float max_distance = 200.0f;      // Max distance in units
    bool aim_at_head = true;          // Target head instead of body
    bool prediction_enabled = false;  // Predict target movement
    float prediction_time = 0.1f;     // Prediction time in seconds
    bool lock_on_key = false;         // Require key press (RMB)
    bool visible_only = true;         // Only aim at visible enemies
};
