#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "../utils/logger.h"

using json = nlohmann::json;

class ConfigValidator {
public:
    ConfigValidator() = default;
    ~ConfigValidator() = default;

    // Validate entire config
    bool ValidateConfig(const json& config);

    // Validate individual sections
    bool ValidateESPConfig(const json& esp_config);
    bool ValidateAimbotConfig(const json& aimbot_config);
    bool ValidateBhopConfig(const json& bhop_config);
    bool ValidateGlowConfig(const json& glow_config);
    bool ValidateSkeletonConfig(const json& skeleton_config);

    // Get error message
    const std::string& GetLastError() const { return last_error; }

private:
    std::string last_error;

    // Helper validation functions
    bool ValidateFloat(float value, float min, float max, const std::string& field);
    bool ValidateInt(int value, int min, int max, const std::string& field);
    bool ValidateBool(bool value, const std::string& field);
    bool ValidateColor(const json& color, const std::string& field);
    bool ValidateString(const std::string& value, const std::string& field);
};
