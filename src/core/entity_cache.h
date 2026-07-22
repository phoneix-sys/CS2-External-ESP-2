#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>
#include <chrono>
#include "../math/vector.h"
#include "../offsets/offsets.h"
#include "../memory/memory.h"

struct EntityData {
    Vector3 origin;
    Vector3 head;
    Vector3 headBone;
    int health = 0;
    int team = 0;
    std::string name;
    bool valid = false;
    bool hasBoneData = false;

    explicit operator bool() const { return valid; }
};

struct GameData {
    std::mutex mtx;
    std::uintptr_t localPlayerPawn = 0;
    std::uintptr_t entList = 0;
    view_matrix_t view_matrix;
    int localTeam = 0;
    std::uintptr_t client = 0;
    DWORD pid = 0;

    bool IsValid() const {
        return localPlayerPawn && entList;
    }
};

class EntityCache {
public:
    explicit EntityCache(UINT refresh_rate_numerator = 144, UINT refresh_rate_denominator = 1, int server_tick_rate = 128);
    ~EntityCache();
    
    const std::vector<EntityData>& GetEntities(uintptr_t client, const GameData& game_data);
    void UpdateAsync();
    void WaitForUpdate();
    
    size_t GetEntityCount() const { return cached_entities.size(); }

private:
    std::vector<EntityData> ReadEntities(uintptr_t client, const GameData& game_data);
    Vector3 GetBonePosition(uintptr_t pawn, int boneId);
    void ProcessEntityData(EntityData& entity);
    
    std::vector<EntityData> cached_entities;
    std::vector<EntityData> prev_entities;
    
    std::mutex mtx;
    std::atomic<bool> is_updating{ false };
    std::unique_ptr<std::thread> update_thread;
    
    int update_interval_ms;
    std::chrono::steady_clock::time_point last_update;
    std::atomic<int> frame_count{ 0 };
    
    static constexpr size_t MAX_ENTITIES = 96;
};