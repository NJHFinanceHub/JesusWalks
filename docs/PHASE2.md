# Phase 2 Build Notes (Enemy Variety, Boss, Save Slots)

**Migration note (February 6, 2026):** This is a legacy Godot milestone record. Active development is now in Unreal Engine 5 (`unreal/TheNazareneAAA`). Use `TODO.md` and `docs/UNREAL_MIGRATION.md` for current execution.

## New gameplay systems
- Enemy variety added through archetype-driven AI:
  - `MELEE_SHIELD`: frontal shield block chance and poise-heavy melee
  - `SPEAR`: longer melee reach and thrust timing
  - `RANGED`: distance management + projectile casting
  - `DEMON`: fast aggression with occasional lunge
  - `BOSS`: named multi-phase encounter with escalating speed/damage
- Named boss encounter:
  - `Legion Sovereign of Gerasa`
  - Spawned in dedicated arena section of Galilee
  - Phase transitions at 66% and 33% health
- Timing polish:
  - Player attacks now use windup/active/recovery windows
  - Enemy attacks include telegraphed strike timing and parry window ratios
  - Parry includes startup timing; block includes perfect-block window
- Animation state machines:
  - Lightweight state machines for both player and enemies
  - Mesh deformation/bobbing used as placeholder animation layer
- Prayer Site save slots:
  - Save slots: `F1`, `F2`, `F3`
  - Load slots: `F5`, `F6`, `F7`
  - Save/load restricted to Prayer Site interaction range
  - Persistence stored at `user://saves/slot_<n>.save`

## New/updated core files
- `scripts/enemy/enemy_ai.gd` (archetypes, boss phase logic, timing windows, enemy serialization)
- `scripts/enemy/enemy_projectile.gd` (ranged projectile)
- `scripts/player/player_controller.gd` (timed combat windows, save/load integration, animation state machine)
- `scripts/world/prayer_site.gd` (save/load prompts + player site binding)
- `scripts/persistence/save_system.gd` (slot-based save/load)
- `scripts/core/galilee_bootstrap.gd` (enemy roster, boss arena, input actions for save/load)
- `scripts/ui/hud.gd` (animation display + updated control hints)

## Controls additions
- `F1-F3`: Save to slot 1-3 (while at Prayer Site)
- `F5-F7`: Load slot 1-3 (while at Prayer Site)

## Known limitations
- Placeholder geometry and procedural mesh deformation stand in for full skeletal animation.
- No dedicated save menu yet; slot control is keyboard-first for prototype speed.
- Godot runtime validation should be run locally to tune balance and timings.

