#define NOMINMAX
#include "bhop.h"
#include <algorithm>
#include <cmath>

BunnyHop::BunnyHop() : was_in_air(false) {
    last_jump_time = std::chrono::steady_clock::now();
    OutputDebugStringA("[BunnyHop] Initialized\n");
}

bool BunnyHop::IsPlayerInAir(uintptr_t local_pawn) {
    if (!local_pawn) return false;
    
    // Check m_fFlags at offset 0x63
    uint32_t flags = mem::ReadMem<uint32_t>(local_pawn + m_fFlags);
    
    // FL_ONGROUND = 1 << 0 (0x1)
    return !(flags & (1 << 0));
}

bool BunnyHop::IsPlayerOnGround(uintptr_t local_pawn) {
    return !IsPlayerInAir(local_pawn);
}

void BunnyHop::AutoStrafe(uintptr_t local_pawn, float yaw_delta) {
    if (!config.auto_strafe || !local_pawn) return;
    
    // Auto strafe implementation
    // This would require mouse movement simulation or memory writes
    // Simplified version - in real implementation:
    // - Read current velocity
    // - Calculate optimal strafe angle
    // - Write movement keys or angles
}

void BunnyHop::ExecuteJump(uintptr_t local_pawn) {
    if (!local_pawn) return;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_jump_time);
    
    if (elapsed.count() < config.jump_delay_ms) return;
    
    // Jump execution would be done through memory writes or input simulation
    // This is a simplified template
    last_jump_time = now;
}

void BunnyHop::OnSpacebarPress(uintptr_t local_pawn) {
    if (!config.enabled || !local_pawn) return;
    
    if (IsPlayerOnGround(local_pawn)) {
        ExecuteJump(local_pawn);
    }
}

void BunnyHop::Update(uintptr_t local_pawn, uintptr_t client) {
    if (!config.enabled || !local_pawn) return;
    
    bool in_air = IsPlayerInAir(local_pawn);
    
    if (in_air && !was_in_air) {
        // Just became airborne
        if (config.auto_strafe) {
            AutoStrafe(local_pawn, 0.0f);
        }
    }
    
    was_in_air = in_air;
}
