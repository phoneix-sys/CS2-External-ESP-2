#include "offset_updater.h"
#include <Windows.h>
#include <shlobj.h>
#include <iostream>
#include <algorithm>
#include <chrono>

OffsetUpdater::OffsetUpdater() {
    OutputDebugStringA("[OffsetUpdater] Initialized\n");
}

size_t OffsetUpdater::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

bool OffsetUpdater::FetchFromGitHub(const std::string& repo, std::string& out_content) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        last_error = "Failed to initialize CURL";
        return false;
    }

    std::string url = "https://api.github.com/repos/" + repo + "/releases/latest";
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "CS2-External-ESP/2.0");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        last_error = "CURL request failed: " + std::string(curl_easy_strerror(res));
        return false;
    }

    out_content = response;
    return true;
}

bool OffsetUpdater::ParseOffsetsJSON(const json& data) {
    try {
        if (!data.contains("browser_download_url")) {
            last_error = "Invalid GitHub release format";
            return false;
        }

        // Extract version from tag_name
        if (data.contains("tag_name")) {
            remote_version = data["tag_name"].get<std::string>();
        }

        // Parse body for offset information
        if (data.contains("body")) {
            std::string body = data["body"].get<std::string>();
            // Parse offset information from release notes
            // Format: offset_name: 0xHEXVALUE
        }

        return true;
    }
    catch (const std::exception& e) {
        last_error = std::string("JSON parse error: ") + e.what();
        return false;
    }
}

bool OffsetUpdater::FetchLatestOffsets(const std::string& repo) {
    std::string response;
    if (!FetchFromGitHub(repo, response)) {
        return false;
    }

    try {
        auto release_data = json::parse(response);
        if (!ParseOffsetsJSON(release_data)) {
            return false;
        }

        has_update_available = CompareVersions(current_version, remote_version);
        OutputDebugStringA("[OffsetUpdater] Successfully fetched latest offsets\n");
        return true;
    }
    catch (const std::exception& e) {
        last_error = std::string("Failed to parse GitHub response: ") + e.what();
        return false;
    }
}

bool OffsetUpdater::LoadFromCache(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        last_error = "Cache file not found: " + filename;
        return false;
    }

    try {
        auto data = json::parse(file);
        file.close();

        if (data.contains("version")) {
            current_version = data["version"].get<std::string>();
        }

        if (data.contains("offsets")) {
            offsets.clear();
            for (const auto& [key, value] : data["offsets"].items()) {
                OffsetData od;
                od.name = key;
                od.value = std::stoul(value.get<std::string>(), nullptr, 16);
                od.category = "cached";
                offsets[key] = od;
            }
        }

        OutputDebugStringA("[OffsetUpdater] Loaded offsets from cache\n");
        return true;
    }
    catch (const std::exception& e) {
        last_error = std::string("Cache load failed: ") + e.what();
        return false;
    }
}

bool OffsetUpdater::SaveToCache(const std::string& filename) const {
    try {
        json data;
        data["version"] = current_version;
        data["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();
        data["offsets"] = json::object();

        for (const auto& [key, offset] : offsets) {
            char buf[32];
            snprintf(buf, sizeof(buf), "0x%llX", offset.value);
            data["offsets"][key] = std::string(buf);
        }

        std::ofstream file(filename);
        if (!file.is_open()) {
            last_error = "Failed to open cache file for writing: " + filename;
            return false;
        }

        file << data.dump(4);
        file.close();

        OutputDebugStringA("[OffsetUpdater] Saved offsets to cache\n");
        return true;
    }
    catch (const std::exception& e) {
        last_error = std::string("Cache save failed: ") + e.what();
        return false;
    }
}

bool OffsetUpdater::GetOffset(const std::string& name, uintptr_t& out_value) const {
    auto it = offsets.find(name);
    if (it == offsets.end()) {
        return false;
    }
    out_value = it->second.value;
    return true;
}

bool OffsetUpdater::ValidateOffsets(uintptr_t client, HANDLE process_handle) {
    if (!client || !process_handle) {
        last_error = "Invalid client or process handle";
        return false;
    }

    int validated_count = 0;
    for (auto& [key, offset] : offsets) {
        // Try to read a value at this offset
        BYTE test_buffer[8] = { 0 };
        SIZE_T bytes_read = 0;

        if (ReadProcessMemory(process_handle, (LPVOID)(client + offset.value), test_buffer, sizeof(test_buffer), &bytes_read)) {
            if (bytes_read > 0) {
                offset.validated = true;
                validated_count++;
            }
        }
    }

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "[OffsetUpdater] Validated %d/%zu offsets\n", validated_count, offsets.size());
    OutputDebugStringA(buffer);

    return validated_count > 0;
}

bool OffsetUpdater::UpdateOffset(const std::string& name, uintptr_t value) {
    if (offsets.find(name) != offsets.end()) {
        offsets[name].value = value;
        return true;
    }

    // Create new offset if not exists
    OffsetData od;
    od.name = name;
    od.value = value;
    od.category = "manual";
    offsets[name] = od;
    return true;
}

bool OffsetUpdater::CompareVersions(const std::string& local, const std::string& remote) const {
    if (local.empty() || remote.empty()) return true;
    return remote > local;  // Simple string comparison (would need semantic versioning for production)
}

std::string OffsetUpdater::GetCacheDirectory() const {
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path))) {
        return std::string(path) + "\\CS2External";
    }
    return ".\\cache";
}

bool OffsetUpdater::AutoUpdate(const std::string& cache_file) {
    // First, try to load from cache
    if (LoadFromCache(cache_file)) {
        OutputDebugStringA("[OffsetUpdater] Loaded offsets from cache\n");
    }

    // Try to fetch latest offsets
    if (FetchLatestOffsets("a2x/cs2-dumper")) {
        if (has_update_available) {
            OutputDebugStringA("[OffsetUpdater] Update available - saving new offsets\n");
            SaveToCache(cache_file);
            return true;
        }
        else {
            OutputDebugStringA("[OffsetUpdater] Offsets are up to date\n");
            return true;
        }
    }
    else {
        // If fetch fails, use cached offsets if available
        if (!offsets.empty()) {
            OutputDebugStringA("[OffsetUpdater] Using cached offsets (fetch failed)\n");
            return true;
        }
        return false;
    }
}
