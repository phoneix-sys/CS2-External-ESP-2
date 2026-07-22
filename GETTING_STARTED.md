# CS2 External ESP - Getting Started Guide

## Quick Start

### 1. Prerequisites
- Windows 10/11 (x64)
- Visual Studio 2019+ with C++ tools
- Counter-Strike 2 installed
- Basic understanding of game hacking concepts

### 2. Compilation

```bash
# Clone/Download the repository
git clone <repo-url>
cd CS2-External-ESP

# Open solution
start CS2External.sln

# Build (Release recommended)
Ctrl+Shift+B  # Build Solution
```

### 3. Running

```bash
# Launch Counter-Strike 2
# Run the compiled executable
bin/Release/CS2External.exe
```

## Configuration

### Via GUI (Recommended)
1. Press `INSERT` to toggle GUI
2. Navigate through tabs
3. Adjust settings in real-time
4. Click "Save Config" in Settings tab

### Via Config File

Create `config.json`:
```json
{
  "esp": {
    "enabled": true,
    "box_style": "outlined",
    "max_distance": 300.0,
    "draw_health": true
  },
  "aimbot": {
    "enabled": false,
    "mode": "smooth",
    "smoothness": 0.5,
    "fov": 20.0
  },
  "bhop": {
    "enabled": false,
    "auto_strafe": true,
    "jump_height": 65.0
  }
}
```

## Features Overview

### ESP Tab
- **Enable ESP**: Toggle ESP rendering
- **Max Distance**: Maximum distance to render (100-2000 units)
- **Box Style**: Choose from 5 box styles
- **Draw Health**: Show health bar
- **Draw Skeleton**: Show bone connections
- **Enemy/Ally Colors**: Customize colors

### Aimbot Tab
- **Enable Aimbot**: Toggle aimbot
- **Aim Mode**: Smooth/Instant/Prediction
- **Smoothness**: Lower = faster, Higher = smoother (0.1-1.0)
- **FOV**: Field of view for target selection (5-180°)
- **Prediction**: Enable velocity prediction
- **Lock Key**: RMB to enable aiming

### Bhop Tab
- **Enable Bhop**: Toggle bhop
- **Auto Strafe**: Auto-direction changing
- **Jump Height**: Jump power (32-150 units)
- **Spacebar**: Press to jump

### Glow Tab
- **Enable Glow**: Toggle glow effects
- **Glow Intensity**: How bright (0.1-5.0)
- **Visible Only**: Only glow visible enemies
- **Through Walls**: Glow through obstacles
- **Colors**: Enemy/Ally glow colors

### Skeleton Tab
- **Enable Skeleton**: Toggle skeleton
- **Line Thickness**: Bone line width (0.5-3.0)
- **Draw Joints**: Show bone joints
- **Skeleton Color**: Customize color

### Settings Tab
- **FPS Limit**: Cap frame rate (30-360 FPS)
- **Show Debug Info**: Display debug statistics
- **Save/Load Config**: Persist settings

## Hotkeys

| Key | Action |
|-----|--------|
| **INSERT** | Toggle GUI visibility |
| **RMB** | Aimbot lock (if enabled) |
| **SPACEBAR** | Jump for bhop (if enabled) |
| **ESC** | Close GUI (in game) |

## Troubleshooting

### "Failed to find CS2 process!"
- Counter-Strike 2 is not running
- Running with wrong architecture (need x64)
- CS2 crashed or is minimized

**Solution**: Launch CS2 and keep it in focus

### "Failed to find client.dll!"
- CS2 hasn't fully loaded
- Using wrong CS2 version

**Solution**: Wait for CS2 to fully load before running

### "Window Creation Failed!"
- Graphics driver issue
- DirectX 11 not available

**Solution**: Update graphics drivers

### ESP not showing
- Check "Enable ESP" in GUI
- Increase "Max Distance"
- Verify entities are in view range

**Solution**: Verify settings and game state

### Aimbot not working
- Check "Enable Aimbot" in GUI
- Verify FOV is not too small
- Check "Visible Only" setting

**Solution**: Adjust FOV and visibility settings

### Performance issues
- Lower FPS limit in Settings
- Reduce entity update frequency
- Disable skeleton rendering
- Use "Outlined" box style

**Solution**: Optimize graphics settings

## Advanced Configuration

### Memory Cache Tuning

Edit `src/memory/memory.h`:
```cpp
static constexpr uint64_t CACHE_TTL_MS = 16;  // Cache lifetime
```

### Entity Update Interval

Edit `src/core/entity_cache.cpp`:
```cpp
update_interval_ms = std::clamp(update_interval_ms, 1, 5);  // 1-5ms
```

### Target FPS

Edit `src/main/cs2ext.cpp`:
```cpp
static constexpr int MAX_TARGET_FPS = 1000;  // Target FPS
```

## Performance Tips

1. **Reduce Max Distance**: Lower ESP render distance
2. **Disable Skeleton**: Unless needed
3. **Use Outlined Box**: Most efficient
4. **Limit FPS**: Set reasonable FPS cap
5. **Async Updates**: Entity cache is already async
6. **Memory Cache**: Automatically optimizes reads

## Understanding Performance

### Frame Time Budget (60 FPS = 16.67ms)
- Entity update: ~1-2ms (async)
- ESP rendering: ~0.8ms
- GUI rendering: ~2-3ms
- ImGui overhead: ~1-2ms
- Buffer swap: ~2-3ms
- **Total: ~7-13ms (good headroom)**

## Common Customizations

### Change default FPS limit
```cpp
static int fps_limit = 120;  // In RenderSettingsTab()
```

### Modify box style default
```cpp
static int box_style = 2;  // 0=Corners, 1=Filled, 2=Outlined, 3=Gradient, 4=3D
```

### Adjust aimbot smoothness
```cpp
static float smoothness = 0.7f;  // Higher = smoother
```

### Change glow intensity
```cpp
static float glow_intensity = 2.0f;  // Brighter
```

## Safety & Detection

### Detection Risk: MODERATE
- External memory reading (driverless)
- No kernel-level access
- Detectable by VAC/EAC
- Use at your own risk on main account

### Security Practices
1. Use on non-prime account
2. Limit feature usage
3. Play naturally
4. Update offsets regularly
5. Monitor for updates

## Support Resources

- **Documentation**: Check README.md and ARCHITECTURE.md
- **Issues**: GitHub issue tracker
- **Community**: Game hacking forums
- **Updates**: Watch repository for new versions

## Next Steps

1. ✅ Compile the project
2. ✅ Launch CS2
3. ✅ Run the executable
4. ✅ Press INSERT for GUI
5. ✅ Explore features in game
6. ✅ Configure to your preference
7. ✅ Save configuration

## FAQ

**Q: Is this detectable?**
A: Yes, anti-cheat systems can detect external hacks.

**Q: Can I use this on my main account?**
A: Not recommended. Use an alternate account only.

**Q: How do I update offsets?**
A: Download from cs2-dumper and update `src/offsets/offsets.h`

**Q: Can I modify the GUI?**
A: Yes! Edit `src/gui/gui_manager.cpp`

**Q: What's the performance overhead?**
A: ~7-10ms per frame on modern hardware

**Q: Can I compile as DLL?**
A: Yes, modify project type in .vcxproj

---

**Last Updated**: 2026-07-22
**Version**: 2.0
**Status**: Fully Functional
