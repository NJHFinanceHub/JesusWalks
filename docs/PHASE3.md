# Phase 3 Build Notes (Polish & Juice Systems)

**Migration note (February 6, 2026):** This is a legacy Godot milestone record. Active development is now in Unreal Engine 5 (`unreal/TheNazareneAAA`). Use `TODO.md` and `docs/UNREAL_MIGRATION.md` for current execution.

## New systems added
- **Audio System** (`scripts/core/audio_manager.gd`)
  - Centralized audio management with SFX pooling
  - Music and ambient sound support
  - Volume control and fade in/out
  - Placeholder structure ready for audio asset integration
  - SFX types defined for all game actions (combat, miracles, environment, UI)

- **Screen Shake System** (`scripts/vfx/camera_shake.gd`)
  - Multiple shake patterns: IMPACT, RUMBLE, EXPLOSION, SUBTLE_TREMOR
  - Trauma-based shake intensity with automatic decay
  - Distance-based shake for positional effects
  - Smooth camera offset and rotation shake

- **Enemy Health Bars** (`scripts/ui/enemy_health_bar.gd`)
  - World-space health/poise bars above enemies
  - Auto-show on damage, auto-hide after delay
  - Always visible when enemy is locked on
  - Follows enemy position in 3D space

- **Floating Damage Numbers** (`scripts/vfx/damage_number.gd`)
  - World-space damage feedback
  - Multiple damage types with distinct visuals:
    - Normal damage (white)
    - Critical damage (gold, larger, emphasized)
    - Healing (green)
    - Poise break (amber)
    - Blocked (gray, slower)
  - Animated rise and fade with random spread

- **VFX Manager** (`scripts/vfx/vfx_manager.gd`)
  - Central coordinator for all visual effects
  - Easy API for spawning damage numbers
  - Screen shake control
  - CanvasLayer-based for proper rendering order

## Integration needed
These systems are ready to use but need to be wired into existing gameplay code:

### Player Controller Integration
- Import AudioManager and call `play_sfx()` on combat actions
- Import VFXManager and call `spawn_damage_number()` when dealing damage
- Call `add_screen_shake()` on successful hits and parries
- Add camera reference to VFXManager during initialization

### Enemy AI Integration
- Call `spawn_damage_number()` when taking damage
- Trigger screen shake on death/redemption
- Play appropriate SFX for attacks and state changes

### HUD Integration
- Instantiate enemy health bars when player locks on
- Update health bars as enemy takes damage
- Hide health bars when lock is broken

### Bootstrap Integration
- Instantiate AudioManager as autoload or scene child
- Instantiate VFXManager and bind to camera
- Wire up audio/VFX systems to player and enemies

## Known limitations
- Audio files are not yet included; system is ready but needs asset integration
- Damage numbers use preload path that needs scene files (currently script-only)
- Screen shake assumes Camera3D is available and properly initialized
- Health bars need camera reference to project 3D positions to screen space

## Next phase suggestions
- Add actual audio assets (combat sounds, ambient loops, music tracks)
- Add particle effects (dust on dodge, impact sparks, redemption glow)
- Add hit-stop/time-freeze on successful parries
- Add combo counter and combat ranking system
- Add more miracle types with unique VFX
- Add environmental audio zones
- Add footstep sounds based on terrain
- Add weather effects (rain, wind)
