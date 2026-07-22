#define NOMINMAX
#include "entity_cache.h"
#include <iostream>
#include <cmath>
#include <algorithm>

constexpr int BONE_HEAD = 6;

struct PrecomputedColors {
    struct RGB {
        int R, G, B, A;
    };
    RGB healthGreen[101];
};

static PrecomputedColors precompColors;

static struct ColorInitializer {
    ColorInitializer() {
        for (int i = 0; i <= 100; i++) {
            float percent = i / 100.0f;
            precompColors.healthGreen[i] = {
                static_cast<int>(255 * (1.0f - percent)),
                static_cast<int>(255 * percent),
                0,
                255
            };
        }
    }
} colorInit;

EntityCache::EntityCache(UINT refresh_rate_numerator, UINT refresh_rate_denominator, int server_tick_rate)
    : is_updating(false), last_update(std::chrono::steady_clock::now()), update_thread(nullptr) {

    double monitor_hz = (refresh_rate_denominator != 0) ?
        static_cast<double>(refresh_rate_numerator) / refresh_rate_denominator : 100.0;

    double server_tick_interval_ms = 1000.0 / server_tick_rate;
    double target_interval_ms = 1.0;

    update_interval_ms = static_cast<int>(target_interval_ms);
    update_interval_ms = std::clamp(update_interval_ms, 1, 5);

    char buffer[256];
    snprintf(buffer, sizeof(buffer),
        "[CS2-External] Entity Cache - Update Interval: %dms (Target FPS: %.0f, Monitor: %.1fHz, Server: %.1fms)\n",
        update_interval_ms, 1000.0 / update_interval_ms, monitor_hz, server_tick_interval_ms);
    OutputDebugStringA(buffer);
}

EntityCache::~EntityCache() {
    is_updating = false;
    std::lock_guard<std::mutex> lock(mtx);
    cached_entities.clear();
    if (update_thread && update_thread->joinable()) {
        update_thread->join();
    }
}

Vector3 EntityCache::GetBonePosition(uintptr_t pawn, int boneId) {
    uintptr_t gameSceneNode = mem::ReadMem<uintptr_t>(pawn + m_pGameSceneNode);
    if (!gameSceneNode) return Vector3{};

    uintptr_t boneMatrix = mem::ReadMem<uintptr_t>(gameSceneNode + m_modelState + 0x80);
    if (!boneMatrix) return Vector3{};

    Vector3 bonePos = mem::ReadMem<Vector3>(boneMatrix + boneId * 32);
    return bonePos;
}

const std::vector<EntityData>& EntityCache::GetEntities(uintptr_t client, const GameData& game_data) {
    const auto now = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update);

    if (elapsed.count() >= update_interval_ms && !is_updating.exchange(true)) {
        if (update_thread && update_thread->joinable()) {
            update_thread->join();
        }

        update_thread = std::make_unique<std::thread>([this, client,
            localPlayerPawn = game_data.localPlayerPawn,
            entList = game_data.entList,
            localTeam = game_data.localTeam]() {
                auto new_entities = ReadEntities(client, GameData{ .localPlayerPawn = localPlayerPawn, .entList = entList, .localTeam = localTeam });
                std::lock_guard<std::mutex> lock(mtx);
                cached_entities = std::move(new_entities);
                last_update = std::chrono::steady_clock::now();
                is_updating = false;
            });
    }

    std::lock_guard<std::mutex> lock(mtx);
    return cached_entities;
}

void EntityCache::WaitForUpdate() {
    if (update_thread && update_thread->joinable()) {
        update_thread->join();
    }
}

std::vector<EntityData> EntityCache::ReadEntities(uintptr_t client, uintptr_t localPlayerPawn,
    uintptr_t entList, int localTeam) {

    std::vector<EntityData> results;
    results.reserve(MAX_ENTITIES);

    for (int i = 0; i < 48; ++i) {
        EntityData data{};
        data.valid = false;

        const uintptr_t listEntry = mem::ReadMem<uintptr_t>(entList + 0x8 * ((i & 0x7FFF) >> 9) + 0x10);
        if (!listEntry) continue;

        const uintptr_t controller = mem::ReadMem<uintptr_t>(listEntry + 0x70 * (i & 0x1FF));
        if (!controller) continue;

        const uintptr_t pawnHandle = mem::ReadMem<uintptr_t>(controller + m_hPawn);
        if (!pawnHandle) continue;

        const uintptr_t pawnEntry = mem::ReadMem<uintptr_t>(entList + 0x8 * ((pawnHandle & 0x7FFF) >> 9) + 0x10);
        if (!pawnEntry) continue;

        const uintptr_t pawn = mem::ReadMem<uintptr_t>(pawnEntry + 0x70 * (pawnHandle & 0x1FF));
        if (!pawn || pawn == localPlayerPawn) continue;

        data.team = mem::ReadMem<int>(pawn + m_iTeamNum);
        if (data.team == localTeam) continue;

        data.health = mem::ReadMem<int>(pawn + m_iHealth);
        if (data.health <= 0 || data.health > 200) continue;

        data.origin = mem::ReadMem<Vector3>(pawn + m_vOldOrigin);
        data.headBone = GetBonePosition(pawn, BONE_HEAD);
        data.hasBoneData = (data.headBone.x != 0 || data.headBone.y != 0 || data.headBone.z != 0);

        if (data.hasBoneData) {
            data.head = data.headBone;
        } else {
            data.head = { data.origin.x, data.origin.y, data.origin.z + 75.0f };
        }

        data.name = mem::ReadString(controller + m_iszPlayerName);
        data.valid = true;

        results.push_back(data);
    }

    return results;
}

std::vector<EntityData> EntityCache::ReadEntities(uintptr_t client, const GameData& game_data) {
    return ReadEntities(client, game_data.localPlayerPawn, game_data.entList, game_data.localTeam);
}

void EntityCache::ProcessEntityData(EntityData& entity) {
    // Additional processing can be added here
    // e.g., distance calculations, visibility checks, etc.
}