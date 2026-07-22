#define NOMINMAX
#include "aimbot.h"
#include <algorithm>
#include <cmath>

Aimbot::Aimbot() : target_entity(nullptr), is_aiming(false) {
    OutputDebugStringA("[Aimbot] Initialized\n");
}

float Aimbot::CalculateDistance(const Vector3& a, const Vector3& b) const {
    return a.Distance(b);
}

float Aimbot::CalculateScreenDistance(const Vector3& a, const Vector3& b, const view_matrix_t& vm) const {
    Vector3 screenA = a.w2s(vm);
    Vector3 screenB = b.w2s(vm);
    
    if (screenA.z < 0.01f || screenB.z < 0.01f) return FLT_MAX;
    
    float dx = screenA.x - screenB.x;
    float dy = screenA.y - screenB.y;
    return std::sqrt(dx * dx + dy * dy);
}

bool Aimbot::IsEntityVisible(const EntityData& entity, const GameData& game_data) {
    if (!settings.visible_only) return true;
    
    Vector3 screenPos = entity.head.w2s(game_data.view_matrix);
    return screenPos.z > 0.01f && screenPos.x > 0 && screenPos.x < 1920 && 
           screenPos.y > 0 && screenPos.y < 1080;
}

EntityData* Aimbot::FindClosestTarget(const GameData& game_data, const std::vector<EntityData>& entities) {
    EntityData* closest = nullptr;
    float closest_distance = settings.fov;
    
    for (auto& entity : const_cast<std::vector<EntityData>&>(entities)) {
        if (!entity.valid) continue;
        
        // Check team
        if (entity.team == game_data.localTeam) continue;
        
        // Check distance
        float dist = CalculateDistance(entity.origin, game_data.view_matrix[3]);
        if (dist > settings.max_distance) continue;
        
        // Check visibility
        if (!IsEntityVisible(entity, game_data)) continue;
        
        // Check FOV
        float screen_dist = CalculateScreenDistance(
            game_data.view_matrix[3],
            settings.aim_at_head ? entity.head : entity.origin,
            game_data.view_matrix
        );
        
        if (screen_dist < closest_distance) {
            closest_distance = screen_dist;
            closest = &entity;
        }
    }
    
    return closest;
}

Vector3 Aimbot::PredictPosition(const EntityData& entity, float time_delta) {
    // Simple prediction: extrapolate position based on last known velocity
    // This is simplified - in real implementation, you'd track velocity over time
    Vector3 predicted = entity.origin;
    
    if (settings.prediction_enabled && time_delta > 0) {
        // Estimated velocity magnitude (simplified)
        float velocity_magnitude = 300.0f; // ~typical CS2 player speed
        Vector3 direction = entity.origin.Normalize();
        predicted = entity.origin + (direction * velocity_magnitude * time_delta);
    }
    
    return predicted;
}

Vector3 Aimbot::SmoothAim(const Vector3& current, const Vector3& target) {
    if (settings.mode == AimMode::INSTANT) {
        return target;
    }
    
    // Smooth aim interpolation
    float smooth_factor = 1.0f - settings.smoothness;
    Vector3 result;
    result.x = current.x + (target.x - current.x) * smooth_factor;
    result.y = current.y + (target.y - current.y) * smooth_factor;
    result.z = current.z + (target.z - current.z) * smooth_factor;
    
    return result;
}

Vector3 Aimbot::CalculateAim(const GameData& game_data, const std::vector<EntityData>& entities) {
    if (settings.mode == AimMode::DISABLED || !is_aiming) {
        return Vector3();
    }
    
    std::lock_guard<std::mutex> lock(mtx);
    
    target_entity = FindClosestTarget(game_data, entities);
    if (!target_entity) return Vector3();
    
    Vector3 target_pos = settings.aim_at_head ? target_entity->head : target_entity->origin;
    
    if (settings.prediction_enabled) {
        target_pos = PredictPosition(*target_entity, settings.prediction_time);
    }
    
    Vector3 aim_angle = target_pos.w2s(game_data.view_matrix);
    return SmoothAim(Vector3(), aim_angle);
}

void Aimbot::Update(const GameData& game_data, const std::vector<EntityData>& entities) {
    if (settings.mode == AimMode::DISABLED) return;
    
    CalculateAim(game_data, entities);
}
