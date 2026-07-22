#pragma once
#include "../renderer/renderer.h"
#include "../core/entity_cache.h"
#include "../features/esp/esp_styles.h"

class ESPRenderer {
public:
    ESPRenderer();
    ~ESPRenderer() = default;
    
    void DrawBoxStyle(const Vector3& head_pos, const Vector3& feet_pos,
                      const GameData& game_data, const EntityData& entity, 
                      BoxStyle style, const RGB& color, float thickness);
    
private:
    void DrawCornersBox(const Vector3& head_pos, const Vector3& feet_pos,
                        const GameData& game_data, float width, float height, 
                        const RGB& color, float thickness);
    
    void DrawFilledBox(const Vector3& head_pos, const Vector3& feet_pos,
                       const GameData& game_data, const RGB& color);
    
    void DrawGradientBox(const EntityData& entity, const GameData& game_data,
                         float width, float height, const Vector3& head_pos);
    
    void Draw3DBox(const EntityData& entity, const GameData& game_data, 
                   const RGB& color, float thickness);
    
    RGB LerpColor(const RGB& a, const RGB& b, float t);
};
