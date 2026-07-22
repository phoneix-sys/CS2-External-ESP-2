#pragma once
#include "../../math/vector.h"
#include "../../core/entity_cache.h"
#include "aim_settings.h"
#include <mutex>

class Aimbot {
public:
    Aimbot();
    ~Aimbot() = default;
    
    void Update(const GameData& game_data, const std::vector<EntityData>& entities);
    Vector3 CalculateAim(const GameData& game_data, const std::vector<EntityData>& entities);
    
    void SetMode(AimMode mode) { settings.mode = mode; }
    void SetSmoothness(float smooth) { settings.smoothness = std::clamp(smooth, 0.1f, 1.0f); }
    void SetFOV(float fov) { settings.fov = std::clamp(fov, 5.0f, 180.0f); }
    void SetMaxDistance(float dist) { settings.max_distance = dist; }
    void SetPrediction(bool enabled) { settings.prediction_enabled = enabled; }
    void SetAimMode(bool head) { settings.aim_at_head = head; }
    
    void OnMouseDown() { is_aiming = true; }
    void OnMouseUp() { is_aiming = false; }
    
    bool IsAiming() const { return is_aiming && settings.mode != AimMode::DISABLED; }
    EntityData* GetTarget() const { return target_entity; }
    
    AimSettings& GetSettings() { return settings; }
    
private:
    AimSettings settings;
    EntityData* target_entity = nullptr;
    Vector3 predicted_position;
    bool is_aiming = false;
    std::mutex mtx;
    
    float CalculateDistance(const Vector3& a, const Vector3& b) const;
    float CalculateScreenDistance(const Vector3& a, const Vector3& b, const view_matrix_t& vm) const;
    EntityData* FindClosestTarget(const GameData& game_data, const std::vector<EntityData>& entities);
    Vector3 PredictPosition(const EntityData& entity, float time_delta);
    bool IsEntityVisible(const EntityData& entity, const GameData& game_data);
    Vector3 SmoothAim(const Vector3& current, const Vector3& target);
};
