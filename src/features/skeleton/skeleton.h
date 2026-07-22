#pragma once
#include "../../core/entity_cache.h"
#include "../../renderer/esp_renderer.h"
#include "skeleton_config.h"
#include <vector>
#include <utility>

struct BoneConnection {
    int from;
    int to;
};

class SkeletonRenderer {
public:
    SkeletonRenderer();
    ~SkeletonRenderer() = default;
    
    void DrawSkeleton(const EntityData& entity, const GameData& game_data);
    void SetEnabled(bool enabled) { config.enabled = enabled; }
    void SetLineThickness(float thickness) { config.line_thickness = std::clamp(thickness, 0.5f, 3.0f); }
    void SetLineColor(const RGB& color) { config.line_color = color; }
    void SetJointSize(int size) { config.joint_size = std::clamp(size, 1, 5); }
    
    bool IsEnabled() const { return config.enabled; }
    SkeletonConfig& GetConfig() { return config; }
    
private:
    SkeletonConfig config;
    static const std::vector<BoneConnection> bone_connections;
    
    Vector3 GetBoneWorldPos(const EntityData& entity, int bone_id, const GameData& game_data);
    Vector3 GetBoneScreenPos(const EntityData& entity, int bone_id, const GameData& game_data);
};
