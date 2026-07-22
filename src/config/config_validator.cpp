#include "config_validator.h"

bool ConfigValidator::ValidateConfig(const json& config) {
    try {
        if (!config.is_object()) {
            last_error = "Config must be a JSON object";
            return false;
        }

        // Validate all sections if they exist
        if (config.contains("esp") && !ValidateESPConfig(config["esp"])) return false;
        if (config.contains("aimbot") && !ValidateAimbotConfig(config["aimbot"])) return false;
        if (config.contains("bhop") && !ValidateBhopConfig(config["bhop"])) return false;
        if (config.contains("glow") && !ValidateGlowConfig(config["glow"])) return false;
        if (config.contains("skeleton") && !ValidateSkeletonConfig(config["skeleton"])) return false;

        LOG_INFO("Config validation successful");
        return true;
    }
    catch (const std::exception& e) {
        last_error = std::string("Config validation exception: ") + e.what();
        LOG_ERROR(last_error);
        return false;
    }
}

bool ConfigValidator::ValidateESPConfig(const json& esp_config) {
    try {
        if (esp_config.contains("enabled") && !ValidateBool(esp_config["enabled"], "esp.enabled")) return false;
        if (esp_config.contains("max_distance") && !ValidateFloat(esp_config["max_distance"], 100.0f, 5000.0f, "esp.max_distance")) return false;
        if (esp_config.contains("line_thickness") && !ValidateFloat(esp_config["line_thickness"], 0.5f, 5.0f, "esp.line_thickness")) return false;
        if (esp_config.contains("enemy_color") && !ValidateColor(esp_config["enemy_color"], "esp.enemy_color")) return false;
        if (esp_config.contains("ally_color") && !ValidateColor(esp_config["ally_color"], "esp.ally_color")) return false;
        return true;
    }
    catch (const std::exception& e) {
        last_error = std::string("ESP config validation failed: ") + e.what();
        LOG_ERROR(last_error);
        return false;
    }
}

bool ConfigValidator::ValidateAimbotConfig(const json& aimbot_config) {
    try {
        if (aimbot_config.contains("enabled") && !ValidateBool(aimbot_config["enabled"], "aimbot.enabled")) return false;
        if (aimbot_config.contains("smoothness") && !ValidateFloat(aimbot_config["smoothness"], 0.1f, 1.0f, "aimbot.smoothness")) return false;
        if (aimbot_config.contains("fov") && !ValidateFloat(aimbot_config["fov"], 5.0f, 180.0f, "aimbot.fov")) return false;
        if (aimbot_config.contains("max_distance") && !ValidateFloat(aimbot_config["max_distance"], 100.0f, 5000.0f, "aimbot.max_distance")) return false;
        if (aimbot_config.contains("prediction_enabled") && !ValidateBool(aimbot_config["prediction_enabled"], "aimbot.prediction_enabled")) return false;
        return true;
    }
    catch (const std::exception& e) {
        last_error = std::string("Aimbot config validation failed: ") + e.what();
        LOG_ERROR(last_error);
        return false;
    }
}

bool ConfigValidator::ValidateBhopConfig(const json& bhop_config) {
    try {
        if (bhop_config.contains("enabled") && !ValidateBool(bhop_config["enabled"], "bhop.enabled")) return false;
        if (bhop_config.contains("auto_strafe") && !ValidateBool(bhop_config["auto_strafe"], "bhop.auto_strafe")) return false;
        if (bhop_config.contains("jump_height") && !ValidateFloat(bhop_config["jump_height"], 32.0f, 150.0f, "bhop.jump_height")) return false;
        if (bhop_config.contains("jump_delay_ms") && !ValidateInt(bhop_config["jump_delay_ms"], 10, 500, "bhop.jump_delay_ms")) return false;
        return true;
    }
    catch (const std::exception& e) {
        last_error = std::string("Bhop config validation failed: ") + e.what();
        LOG_ERROR(last_error);
        return false;
    }
}

bool ConfigValidator::ValidateGlowConfig(const json& glow_config) {
    try {
        if (glow_config.contains("enabled") && !ValidateBool(glow_config["enabled"], "glow.enabled")) return false;
        if (glow_config.contains("intensity") && !ValidateFloat(glow_config["intensity"], 0.1f, 5.0f, "glow.intensity")) return false;
        if (glow_config.contains("through_walls") && !ValidateBool(glow_config["through_walls"], "glow.through_walls")) return false;
        return true;
    }
    catch (const std::exception& e) {
        last_error = std::string("Glow config validation failed: ") + e.what();
        LOG_ERROR(last_error);
        return false;
    }
}

bool ConfigValidator::ValidateSkeletonConfig(const json& skeleton_config) {
    try {
        if (skeleton_config.contains("enabled") && !ValidateBool(skeleton_config["enabled"], "skeleton.enabled")) return false;
        if (skeleton_config.contains("line_thickness") && !ValidateFloat(skeleton_config["line_thickness"], 0.5f, 3.0f, "skeleton.line_thickness")) return false;
        if (skeleton_config.contains("joint_size") && !ValidateInt(skeleton_config["joint_size"], 1, 10, "skeleton.joint_size")) return false;
        return true;
    }
    catch (const std::exception& e) {
        last_error = std::string("Skeleton config validation failed: ") + e.what();
        LOG_ERROR(last_error);
        return false;
    }
}

bool ConfigValidator::ValidateFloat(float value, float min, float max, const std::string& field) {
    if (value < min || value > max) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "Float validation failed for %s: %f (range: %f-%f)", field.c_str(), value, min, max);
        last_error = buffer;
        LOG_WARNING(last_error);
        return false;
    }
    return true;
}

bool ConfigValidator::ValidateInt(int value, int min, int max, const std::string& field) {
    if (value < min || value > max) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "Int validation failed for %s: %d (range: %d-%d)", field.c_str(), value, min, max);
        last_error = buffer;
        LOG_WARNING(last_error);
        return false;
    }
    return true;
}

bool ConfigValidator::ValidateBool(bool value, const std::string& field) {
    // Bool is always valid
    return true;
}

bool ConfigValidator::ValidateColor(const json& color, const std::string& field) {
    try {
        if (!color.is_array() || color.size() != 4) {
            last_error = "Color must be RGBA array [R, G, B, A]";
            LOG_WARNING(last_error);
            return false;
        }
        for (size_t i = 0; i < 4; i++) {
            int val = color[i].get<int>();
            if (val < 0 || val > 255) {
                char buffer[256];
                snprintf(buffer, sizeof(buffer), "Color component out of range for %s[%zu]: %d", field.c_str(), i, val);
                last_error = buffer;
                LOG_WARNING(last_error);
                return false;
            }
        }
        return true;
    }
    catch (const std::exception& e) {
        last_error = std::string("Color validation exception: ") + e.what();
        LOG_WARNING(last_error);
        return false;
    }
}

bool ConfigValidator::ValidateString(const std::string& value, const std::string& field) {
    if (value.empty()) {
        last_error = "String field " + field + " cannot be empty";
        LOG_WARNING(last_error);
        return false;
    }
    return true;
}
