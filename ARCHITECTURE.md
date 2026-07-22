# CS2 External ESP v2.0 - Architecture & Implementation Guide

## System Architecture

### 1. Entity Management (Core)

**EntityCache System**
```
Input: Game Memory (localPlayerPawn, entList, view_matrix)
   ↓
Async Reader Thread
   ├─ Read entity controllers
   ├─ Validate team/health
   ├─ Get bone positions
   └─ Cache results
   ↓
Output: std::vector<EntityData>
```

**Key Optimizations:**
- Threaded reading with 1-6ms updates
- Precomputed bone lookups
- Delta compression for cached data
- LRU cache for bone positions

### 2. Memory Subsystem

**Three-tier approach:**
```
Direct Memory Read (rwm calls)
   ↓ (cache miss)
MemoryCache (LRU, 16ms TTL)
   ↓ (full cache)
Precomputed Values (colors, offsets)
```

**Batch Operations:**
```cpp
vector<uint32_t> healths = mem::ReadBatch<uint32_t>(base, health_offsets);
```

### 3. Rendering Pipeline

```
Input: EntityData vector
   ↓
Screen Projection (w2s)
   ↓
Style Selection
├─ Corners Box
├─ Filled Box
├─ Gradient Box
├─ 3D Box
└─ Outlined Box
   ↓
Health Bar Rendering
   ↓
ImGui Render Queue
   ↓
DirectX 11 Presentation
```

### 4. Feature Architecture

#### Aimbot
```
Target Selection
   ├─ FOV check (cone)
   ├─ Distance check
   ├─ Visibility check
   └─ Team check
   ↓
Aim Calculation
   ├─ Head vs Body
   ├─ Prediction (velocity)
   └─ Smoothness lerp
   ↓
Output: Vector3 aim_angle
```

#### Bunny Hop
```
Input: Player flags (on ground check)
   ↓
Jump Timing
   ├─ Detect ground state change
   ├─ Auto strafe direction
   └─ Delay throttling
   ↓
Output: Jump command
```

#### Glow
```
Entity Loop
   ├─ Visibility check
   ├─ Team differentiation
   └─ Write glow entity
```

#### Skeleton
```
Bone Retrieval
   ├─ Read bone matrix
   ├─ Cache lookups
   └─ Project to screen
   ↓
Connection Drawing
   ├─ Line rendering
   └─ Joint circles
```

## Implementation Details

### Entity Reading Flow

```cpp
// cs2ext.cpp main loop
while (running) {
    // 1. Update game data
    game_data.localPlayerPawn = mem::ReadMem(client + dwLocalPlayerPawn);
    game_data.view_matrix = mem::ReadMem(client + dwViewMatrix);
    
    // 2. Get cached entities (async)
    const auto& entities = entity_cache->GetEntities(client, game_data);
    
    // 3. Update all features
    aimbot->Update(game_data, entities);
    glow->Update(client, game_data, entities);
    
    // 4. Render
    ImGui::NewFrame();
    render::DrawESP(entities, game_data);
    gui->Render();
    ImGui::EndFrame();
}
```

### Memory Cache Example

```cpp
// Fast repeated reads
mem::MemoryCache cache;
Vector3 pos1 = cache.ReadCached<Vector3>(entity + m_vOldOrigin);
Vector3 pos2 = cache.ReadCached<Vector3>(entity + m_vOldOrigin); // From cache
```

### GUI Tab System

```
GUIManager::Render()
   ├─ ImGui::BeginTabBar()
   ├─ Tab 1: ESP Settings
   │  ├─ Box style selector
   │  ├─ Distance slider
   │  └─ Color pickers
   ├─ Tab 2: Aimbot
   │  ├─ Mode selector
   │  ├─ Smoothness slider
   │  └─ FOV adjuster
   ├─ ... (more tabs)
   └─ ImGui::EndTabBar()
```

## Offset Management

**Current Offsets** (from cs2-dumper):
```cpp
dwLocalPlayerPawn = 0x2069B50
dwEntityList = 0x24AF268
dwViewMatrix = 0x230FF20
m_iHealth = 0x354
m_iTeamNum = 0x3F3
m_vOldOrigin = 0x1588
```

**Update Process:**
1. Check cs2-dumper releases
2. Update `src/offsets/offsets.h`
3. Test in-game
4. Commit with version info

## Thread Safety

**Protected Resources:**
```cpp
// game_data
std::lock_guard<std::mutex> lock(game_data.mtx);
game_data.localPlayerPawn = ...; // Safe write

// entity_cache
std::lock_guard<std::mutex> lock(entity_cache->mtx);
cached_entities = ...; // Safe update
```

## Performance Profiling

**Metrics to Track:**
- Entity read time: `ReadEntities()` duration
- Cache hit rate: cache hits / total reads
- Memory stalls: blocked wait time
- Render time: ImGui rendering duration
- Frame time: Full frame time

**Tools:**
- Visual Studio Profiler
- Windows Performance Toolkit
- Custom timing code in main loop

## Common Issues & Solutions

### Issue: High entity read time
**Solution**: Reduce update frequency or batch reads

### Issue: ESP lag/stutter
**Solution**: 
- Check entity cache interval
- Reduce render queue size
- Lower FPS limit

### Issue: Aimbot not locking
**Solution**:
- Check FOV setting
- Verify visibility check
- Check team comparison

### Issue: Memory read failures
**Solution**:
- Verify offset values
- Check process handle
- Update offsets from dumper

## Future Enhancements

### Short-term
- SIMD vectorization for math
- GPU-accelerated rendering
- Config profiles system

### Long-term
- Machine learning for aimbot
- Behavior analytics
- Anti-cheat evasion
- Network optimization

---

**Last Updated**: 2026-07-22
