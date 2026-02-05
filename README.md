# The Nazarene
RPG-inspired biblical action RPG prototype built in Godot 4.

## Current milestone
Phase 2 vertical slice is implemented:
- Core third-person movement + camera
- Stamina combat with timing windows (windup/active/recovery)
- Basic lock-on targeting
- Galilee prototype zone
- Enemy archetypes:
  - Roman Shieldbearer (melee + shield block behavior)
  - Roman Spearman (extended reach)
  - Roman Slinger (ranged projectile behavior)
  - Unclean Spirit (aggressive demon behavior)
  - Named boss: Legion Sovereign of Gerasa (multi-phase)
- Prayer Site rest/reset system
- Prayer Site save slots and persistence (F1-F3 save, F5-F7 load)
- Simple animation state machines for player + enemies
- Healing miracle using Faith resource

See `docs/PHASE1.md` and `docs/PHASE2.md` for control map and architecture notes.
See `docs/AAA_REVIEW.md` for a verified feature audit and full-game (AAA) scope gaps.

## Run
1. Install Godot 4.2+.
2. Open this folder as a Godot project (`project.godot`).
3. Run the main scene (`scenes/GalileeRegion.tscn`).
