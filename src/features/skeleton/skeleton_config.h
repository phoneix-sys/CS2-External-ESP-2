#pragma once
#include "../../math/vector.h"

struct RGB {
    int R, G, B, A;
};

struct SkeletonConfig {
    bool enabled = true;
    float line_thickness = 1.5f;            // 0.5 to 3.0
    RGB line_color = { 0, 255, 255, 255 }; // Cyan
    RGB joint_color = { 255, 255, 0, 255 }; // Yellow
    int joint_size = 2;
    bool draw_joints = true;
    bool draw_lines = true;
};
