#define NOMINMAX
#include "skeleton.h"
#include "../../offsets/offsets.h"
#include "../../memory/memory.h"

const std::vector<BoneConnection> SkeletonRenderer::bone_connections = {
    // Head to spine
    {6, 7},    // Head -> Neck
    {7, 8},    // Neck -> Upper Spine
    {8, 9},    // Upper Spine -> Middle Spine
    
    // Left arm
    {8, 10},   // Shoulder -> Left Upper Arm
    {10, 11},  // Left Upper Arm -> Left Forearm
    {11, 12},  // Left Forearm -> Left Hand
    
    // Right arm
    {8, 13},   // Shoulder -> Right Upper Arm
    {13, 14},  // Right Upper Arm -> Right Forearm
    {14, 15},  // Right Forearm -> Right Hand
    
    // Left leg
    {9, 1},    // Spine -> Left Hip
    {1, 2},    // Left Hip -> Left Knee
    {2, 3},    // Left Knee -> Left Foot
    
    // Right leg
    {9, 4},    // Spine -> Right Hip
    {4, 5},    // Right Hip -> Right Knee
    {5, 6}     // Right Knee -> Right Foot
};

SkeletonRenderer::SkeletonRenderer() {
    OutputDebugStringA("[SkeletonRenderer] Initialized\n");
}

Vector3 SkeletonRenderer::GetBoneWorldPos(const EntityData& entity, int bone_id, const GameData& game_data) {
    // This would require accessing the bone matrix from entity pawn
    // Simplified template
    return entity.origin;
}

Vector3 SkeletonRenderer::GetBoneScreenPos(const EntityData& entity, int bone_id, const GameData& game_data) {
    Vector3 world_pos = GetBoneWorldPos(entity, bone_id, game_data);
    return world_pos.w2s(game_data.view_matrix);
}

void SkeletonRenderer::DrawSkeleton(const EntityData& entity, const GameData& game_data) {
    if (!config.enabled || !entity.valid || !entity.hasBoneData) return;
    
    // Draw bone connections
    if (config.draw_lines) {
        for (const auto& connection : bone_connections) {
            Vector3 pos1 = GetBoneScreenPos(entity, connection.from, game_data);
            Vector3 pos2 = GetBoneScreenPos(entity, connection.to, game_data);
            
            if (pos1.z > 0.01f && pos2.z > 0.01f) {
                // DrawLine would be called here with renderer
                // render::DrawLine(pos1.x, pos1.y, pos2.x, pos2.y, config.line_color, config.line_thickness);
            }
        }
    }
    
    // Draw joints
    if (config.draw_joints) {
        for (const auto& connection : bone_connections) {
            Vector3 pos = GetBoneScreenPos(entity, connection.from, game_data);
            if (pos.z > 0.01f) {
                // DrawFilledCircle would be called here
                // render::DrawFilledCircle(pos.x, pos.y, config.joint_size, config.joint_color);
            }
        }
    }
}
