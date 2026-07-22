#define NOMINMAX
#include "glow.h"
#include <algorithm>

Glow::Glow() {
    OutputDebugStringA("[Glow] Initialized\n");
}

uintptr_t Glow::GetGlowEntity(uintptr_t pawn) {
    if (!pawn) return 0;
    
    // Simplified glow entity retrieval
    // In real implementation, you'd navigate the glow manager
    // using dwGlowManager offset
    return pawn;
}

bool Glow::IsEntityOnScreen(const EntityData& entity, const GameData& game_data) {
    Vector3 screenPos = entity.head.w2s(game_data.view_matrix);
    return screenPos.z > 0.01f && screenPos.x > 0 && screenPos.x < 1920 && 
           screenPos.y > 0 && screenPos.y < 1080;
}

void Glow::ApplyGlow(uintptr_t pawn, const RGB& color, float intensity) {
    if (!pawn || !settings.enabled) return;
    
    // Glow application would involve writing to glow entity structures
    // This is a template for future implementation
    uintptr_t glow_entity = GetGlowEntity(pawn);
    if (glow_entity) {
        glowed_entities.push_back(glow_entity);
    }
}

void Glow::RemoveGlow(uintptr_t pawn) {
    auto it = std::find(glowed_entities.begin(), glowed_entities.end(), pawn);
    if (it != glowed_entities.end()) {
        glowed_entities.erase(it);
    }
}

void Glow::Update(uintptr_t client, const GameData& game_data, const std::vector<EntityData>& entities) {
    if (!settings.enabled) {
        // Remove all glows
        for (auto entity : glowed_entities) {
            RemoveGlow(entity);
        }
        glowed_entities.clear();
        return;
    }
    
    for (const auto& entity : entities) {
        if (!entity.valid) continue;
        
        if (settings.visualized_only && !IsEntityOnScreen(entity, game_data)) {
            continue;
        }
        
        RGB glow_color = (entity.team == game_data.localTeam) ? 
            settings.ally_color : settings.enemy_color;
        
        ApplyGlow(entity.origin, glow_color, settings.intensity);
    }
}
