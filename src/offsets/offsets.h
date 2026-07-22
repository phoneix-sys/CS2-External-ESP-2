#pragma once

#include <cstdint>
#include <cstddef>

// github.com/a2x/cs2-dumper

// Main offsets
constexpr uintptr_t dwCSGOInput = 0x2319FC0;
constexpr uintptr_t dwEntityList = 0x24AF268;
constexpr uintptr_t dwGameEntitySystem = 0x24AF268;
constexpr uintptr_t dwGameEntitySystem_highestEntityIndex = 0x20A0;
constexpr uintptr_t dwGameRules = 0x230DEE0;
constexpr uintptr_t dwGlobalVars = 0x205E5C0;
constexpr uintptr_t dwGlowManager = 0x230ACE8;
constexpr uintptr_t dwLocalPlayerController = 0x22F4188;
constexpr uintptr_t dwLocalPlayerPawn = 0x2069B50;
constexpr uintptr_t dwPlantedC4 = 0x2317740;
constexpr uintptr_t dwPrediction = 0x2069A60;
constexpr uintptr_t dwSensitivity = 0x230B7F8;
constexpr uintptr_t dwSensitivity_sensitivity = 0x58;
constexpr uintptr_t dwViewAngles = 0x231A648;
constexpr uintptr_t dwViewMatrix = 0x230FF20;
constexpr uintptr_t dwViewRender = 0x2310338;
constexpr uintptr_t dwWeaponC4 = 0x2298F00;

// Player - Pawn offsets
constexpr uintptr_t m_iHealth = 0x354;
constexpr uintptr_t m_hPlayerPawn = 0x90C;

constexpr uintptr_t m_iObserverMode = 0x48;
constexpr uintptr_t m_pObserverServices = 0x13F0;
constexpr uintptr_t m_hObserverTarget = 0x4C;
constexpr uintptr_t m_hObserverPawn = 0x910;
constexpr uintptr_t m_hController = 0x15A0;

constexpr uintptr_t m_iTeamNum = 0x3F3;
constexpr uintptr_t m_hPawn = 0x6C4;
constexpr uintptr_t m_vOldOrigin = 0x1588;
constexpr uintptr_t m_iszPlayerName = 0x6F8;
constexpr uintptr_t m_pGameSceneNode = 0x338;
constexpr uintptr_t m_fFlags = 0x63;

// Bone - Anim offsets
constexpr uintptr_t m_modelState = 0x160;
constexpr uintptr_t dwBoneMatrix = 0x160;
constexpr uintptr_t m_nForceBone = 0xF78;

// Additional offsets for features
constexpr uintptr_t m_flSimulationTime = 0x288;
constexpr uintptr_t m_vecVelocity = 0xBC;
constexpr uintptr_t m_vecAbsVelocity = 0xBC;