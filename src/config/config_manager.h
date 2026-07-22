#pragma once
#include "../features/esp/esp_styles.h"
#include "../features/aimbot/aimbot.h"
#include "../features/bhop/bhop.h"
#include "../features/glow/glow.h"
#include "../features/skeleton/skeleton.h"
#include "../core/entity_cache.h"
#include <json/json.h>

class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager() = default;
    
    void SaveConfig(const std::string& filename);
    void LoadConfig(const std::string& filename);
    void SetDefaults();
    
    Json::Value GetESPConfig() const { return root["esp"]; }
    Json::Value GetAimbotConfig() const { return root["aimbot"]; }
    Json::Value GetBhopConfig() const { return root["bhop"]; }
    Json::Value GetGlowConfig() const { return root["glow"]; }
    Json::Value GetSkeletonConfig() const { return root["skeleton"]; }
    
private:
    Json::Value root;
};
