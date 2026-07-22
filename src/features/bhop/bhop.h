#pragma once
#include "../../math/vector.h"
#include "bhop_config.h"
#include "../../offsets/offsets.h"
#include "../../memory/memory.h"
#include <chrono>

class BunnyHop {
public:
    BunnyHop();
    ~BunnyHop() = default;
    
    void Update(uintptr_t local_pawn, uintptr_t client);
    void OnSpacebarPress(uintptr_t local_pawn);
    
    void SetEnabled(bool enabled) { config.enabled = enabled; }
    void SetAutoStrafe(bool enabled) { config.auto_strafe = enabled; }
    void SetJumpHeight(float height) { config.jump_height = std::clamp(height, 32.0f, 150.0f); }
    void SetPerfectTiming(bool enabled) { config.perfect_timing = enabled; }
    
    bool IsEnabled() const { return config.enabled; }
    BhopConfig& GetConfig() { return config; }
    
private:
    BhopConfig config;
    bool was_in_air = false;
    std::chrono::steady_clock::time_point last_jump_time;
    
    bool IsPlayerInAir(uintptr_t local_pawn);
    bool IsPlayerOnGround(uintptr_t local_pawn);
    void AutoStrafe(uintptr_t local_pawn, float yaw_delta);
    void ExecuteJump(uintptr_t local_pawn);
};
