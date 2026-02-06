# Phase 1 Build Notes (Galilee Vertical Slice)

**Migration note (February 6, 2026):** This is a legacy Godot milestone record. Active development is now in Unreal Engine 5 (`unreal/TheNazareneAAA`). Use `TODO.md` and `docs/UNREAL_MIGRATION.md` for current execution.

## Included systems
- Third-person player controller with camera orbit and lock-on targeting.
- Stamina-based combat loop:
  - `Light attack` (LMB)
  - `Heavy attack` (RMB)
  - `Dodge` (Space) with brief invulnerability
  - `Block` (Shift)
  - `Parry` (F) with riposte windows
- Basic enemy AI:
  - Patrol idle/chase/attack behavior
  - Poise and stagger states
  - Parry vulnerability
  - Redemption flow (enemy is redeemed, not killed)
- Prayer Site system:
  - Interaction prompt near altar
  - Rest action (`E`) to fully recover player
  - Enemy reset to spawn on rest
- Miracle system (initial):
  - Healing miracle (`R`)
  - Consumes Faith
  - Cooldown-based
- Galilee region prototype environment:
  - Playable terrain, props, sea backdrop, ambient lighting
  - Spawned enemy encounters
  - HUD for vitals and contextual prompts

## Controls
- `WASD` / Arrow keys: movement
- `Mouse`: camera orbit
- `Q`: lock-on nearest enemy
- `LMB`: light attack
- `RMB`: heavy attack
- `Space`: dodge roll burst
- `Shift`: hold block
- `F`: parry
- `R`: healing miracle
- `E`: interact/rest at prayer site
- `Esc`: toggle mouse capture

## Main implementation files
- `project.godot`
- `scenes/GalileeRegion.tscn`
- `scripts/core/galilee_bootstrap.gd`
- `scripts/player/player_controller.gd`
- `scripts/enemy/enemy_ai.gd`
- `scripts/world/prayer_site.gd`
- `scripts/ui/hud.gd`

## Next phase suggestions
- Add a proper state machine/animation graph for player and enemies.
- Introduce enemy archetypes (spear, shield, ranged, demon variants).
- Add navigation mesh and patrol routes.
- Add save slots and prayer site persistence state.
- Build first named boss encounter in Galilee.

