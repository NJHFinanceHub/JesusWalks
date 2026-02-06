# The Nazarene
RPG-inspired biblical action RPG built in Godot 4.

## Current milestone
Phase 3 campaign build is implemented:
- Core third-person movement + camera
- Stamina combat with timing windows (windup/active/recovery)
- Basic lock-on targeting
- Four-region pilgrimage campaign (Galilee, Decapolis, Wilderness, Jerusalem)
- Enemy archetypes:
  - Roman Shieldbearer (melee + shield block behavior)
  - Roman Spearman (extended reach)
  - Roman Slinger (ranged projectile behavior)
  - Unclean Spirit (aggressive demon behavior)
  - Named bosses per region (multi-phase)
- Prayer Site rest/reset system
- Prayer Site save slots and persistence (F1-F3 save, F5-F7 load)
- Campaign checkpoints and travel gates between regions
- Simple animation state machines for player + enemies
- Miracles unlocked through progression (Healing, Blessing, Radiance)
- Main menu, pause menu, and credits

See `docs/PHASE1.md`, `docs/PHASE2.md`, and `docs/PHASE3.md` for control map and architecture notes.
See `docs/AAA_REVIEW.md` for a verified feature audit and full-game (AAA) scope gaps.

## Run
1. Install Godot 4.2+.
2. Open this folder as a Godot project (`project.godot`).
3. Run the main scene (`scenes/MainMenu.tscn`).

## Unreal Engine Migration (AAA Track)
An Unreal Engine 5 C++ migration scaffold is now included at `unreal/TheNazareneAAA`.

Included in the Unreal migration:
- Campaign game mode with four region definitions and boss-gated progression
- Third-person player character with lock-on, stamina combat timing, miracles, prayer-site rest flow, and slot input bindings
- Enemy archetype class with state-driven AI, parry/riposte, and boss phases
- Prayer Site and Travel Gate interaction actors
- Save-game payload model and checkpoint/slot save subsystem
- Runtime HUD for vitals/objective/context text

See `docs/UNREAL_MIGRATION.md` and `unreal/TheNazareneAAA/README.md` for setup and mapping details.
