#pragma once
#include "../../core/entity_cache.h"
#include "glow_settings.h"
#include "../../offsets/offsets.h"
#include "../../memory/memory.h"
#include <vector>

class Glow {
public:
    Glow();
    ~Glow() = default;
    
    void Update(uintptr_t client, const GameData& game_data, const std::vector<EntityData>& entities);
    void ApplyGlow(uintptr_t pawn, const RGB& color, float intensity);
    void RemoveGlow(uintptr_t pawn);
    
    void SetEnabled(bool enabled) { settings.enabled = enabled; }
    void SetIntensity(float intensity) { settings.intensity = std::clamp(intensity, 0.1f, 5.0f); }
    void SetEnemyColor(const RGB& color) { settings.enemy_color = color; }
    void SetAllyColor(const RGB& color) { settings.ally_color = color; }
    void SetThroughWalls(bool enabled) { settings.through_walls = enabled; }
    
    bool IsEnabled() const { return settings.enabled; }
    GlowSettings& GetSettings() { return settings; }
    
private:
    GlowSettings settings;
    std::vector<uintptr_t> glowed_entities;
    
    uintptr_t GetGlowEntity(uintptr_t pawn);
    bool IsEntityOnScreen(const EntityData& entity, const GameData& game_data);
};
