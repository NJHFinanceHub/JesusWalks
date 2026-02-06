# The Nazarene
Biblical action RPG in active migration to Unreal Engine 5.

## Migration status (February 6, 2026)
- Active development target: Unreal Engine 5 C++ project in `unreal/TheNazareneAAA`.
- Godot content in `project.godot`, `scenes/`, and `scripts/` is legacy prototype reference material for parity checks.
- All new gameplay and production work should be planned against Unreal tasks in `TODO.md` and `docs/UNREAL_MIGRATION.md`.

## Unreal quick start (primary path)
1. Install Unreal Engine 5.4+ and Visual Studio with the C++ toolchain.
2. Open `unreal/TheNazareneAAA/TheNazareneAAA.uproject`.
3. Generate project files if prompted, then build.
4. Launch in Unreal Editor and run PIE.

## Current Unreal implementation baseline
- Campaign game mode with region progression and boss-gated travel.
- Third-person player character with lock-on, stamina combat timing, miracles, and save/load bindings.
- Enemy archetype AI with state transitions plus AI perception/controller behavior-tree hooks.
- Prayer Site and Travel Gate interaction actors.
- Save payload model with checkpoint and slot persistence subsystem.
- UMG HUD + pause menu stack with in-UI save/load slot flow and summaries.
- Animation runtime classes for player/enemy animation blueprint state machines.
- Audio/VFX integration points (SoundBase + Niagara hooks) and region-specific environment spawning.

## Legacy Godot notes
- Historical milestone docs: `docs/PHASE1.md`, `docs/PHASE2.md`, `docs/PHASE3.md`.
- Historical audit docs: `docs/AAA_REVIEW.md`, `docs/CODE_REVIEW_REPORT.md`.
- Godot project can still be opened for reference using `project.godot`, but it is not the primary delivery path.

## Documentation map
- Migration plan: `docs/UNREAL_MIGRATION.md`
- Active work queue: `TODO.md`
- Unreal project setup: `unreal/TheNazareneAAA/README.md`
- Campaign level bootstrap: `docs/UNREAL_LEVEL_BOOTSTRAP.md`
- Gameplay validation: `docs/UNREAL_GAMEPLAY_VALIDATION.md`
- Packaging/profile pipeline: `docs/PACKAGING_PROFILE_PIPELINE.md`
- GAS evaluation: `docs/GAS_ADOPTION_EVALUATION.md`
- Networking scope: `docs/NETWORKING_SCOPE.md`
- Narrative scope: `docs/PLOT.md`
- Character roster: `docs/CHARACTER_LIBRARY.md`
