#pragma once

#include <string>
#include <map>
#include <vector>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <fstream>

using json = nlohmann::json;

struct OffsetData {
    std::string name;
    uintptr_t value;
    std::string category;
    std::string description;
    bool validated = false;
};

class OffsetUpdater {
public:
    OffsetUpdater();
    ~OffsetUpdater() = default;

    // Fetch latest offsets from cs2-dumper GitHub
    bool FetchLatestOffsets(const std::string& repo = "a2x/cs2-dumper");
    
    // Load offsets from cache file
    bool LoadFromCache(const std::string& filename = "offsets_cache.json");
    
    // Save offsets to cache file
    bool SaveToCache(const std::string& filename = "offsets_cache.json") const;
    
    // Get offset by name
    bool GetOffset(const std::string& name, uintptr_t& out_value) const;
    
    // Validate offsets in memory
    bool ValidateOffsets(uintptr_t client, HANDLE process_handle);
    
    // Update a single offset
    bool UpdateOffset(const std::string& name, uintptr_t value);
    
    // Get all offsets
    const std::map<std::string, OffsetData>& GetAllOffsets() const { return offsets; }
    
    // Get version
    const std::string& GetVersion() const { return current_version; }
    
    // Check if update available
    bool IsUpdateAvailable() const { return has_update_available; }
    
    // Get last error message
    const std::string& GetLastError() const { return last_error; }
    
    // Auto-update from GitHub (checks and updates if needed)
    bool AutoUpdate(const std::string& cache_file = "offsets_cache.json");

private:
    std::map<std::string, OffsetData> offsets;
    std::string current_version;
    std::string remote_version;
    bool has_update_available = false;
    std::string last_error;
    
    // Helper methods
    bool ParseOffsetsJSON(const json& data);
    bool FetchFromGitHub(const std::string& repo, std::string& out_content);
    bool CompareVersions(const std::string& local, const std::string& remote) const;
    std::string GetCacheDirectory() const;
    
    // CURL callback for HTTP requests
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
};
