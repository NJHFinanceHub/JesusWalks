# Project TODO

## Direction (February 6, 2026)
- Primary engine: Unreal Engine 5 C++ (`unreal/TheNazareneAAA`).
- Godot prototype (`project.godot`, `scenes/`, `scripts/`) is legacy reference only.
- New tasks should target Unreal implementation unless explicitly marked as legacy maintenance.

## Bot update command (run at the end of every task)
```sh
bash -lc 'printf "\n## Updates (%s)\n- [ ] <new todo> (@claude|@codex)\n" "$(date +%F)" >> TODO.md'
```

## Completed baseline (legacy + migration)
- Godot prototype established combat, lock-on, enemy archetypes, prayer sites, save slots, and campaign progression flow.
- Unreal C++ scaffold now mirrors major gameplay systems:
  - `ANazareneCampaignGameMode`, `ANazarenePlayerCharacter`, `ANazareneEnemyCharacter`
  - `ANazarenePrayerSite`, `ANazareneTravelGate`
  - `UNazareneSaveSubsystem`, `UNazareneSaveGame`
  - `ANazareneHUD`
- Core migration docs and code mapping are documented in `docs/UNREAL_MIGRATION.md`.

## Active Unreal tasks (assignments)
- [x] Create initial playable `.umap` campaign level and set `NazareneCampaignGameMode` defaults. (@codex)
- [x] Replace direct input bindings with Enhanced Input assets and mapping contexts. (@claude)
- [x] Build UMG/CommonUI HUD + menu stack to replace code-only HUD flow. (@codex)
- [x] Implement Behavior Tree + AI Perception pipeline for enemy decision logic. (@claude)
- [x] Author Animation Blueprint state machines and retargeted skeletal animation sets for player and enemies. (@codex)
- [x] Integrate combat/UI/audio with Unreal assets (SoundCues/MetaSounds, Niagara, materials). (@claude)
- [x] Add save/load front-end menu flow and slot summaries in Unreal UI. (@codex)
- [x] Run Unreal gameplay validation pass and document balance tuning against migration parity targets. (@claude)
- [x] Define packaging/profile pipeline (Win64 target, perf budgets, crash logs). (@codex)

## Backlog (after parity)
- [x] Evaluate GAS adoption for miracles, cooldowns, buffs, and status effects. (@claude)
- [x] Define networking scope (single-player only vs co-op) and technical implications. (@codex)
- [x] Replace placeholder geometry with production environments for all four regions. (@claude)

## Production backlog (full game)
- [ ] Refactor campaign flow into dedicated subsystems (Campaign/Quest/Narrative) and keep `ANazareneCampaignGameMode` as thin bootstrap only. (@codex)
- [ ] Migrate hard-coded chapter/region definitions to DataAssets/DataTables with stable content IDs and authoring workflow docs. (@codex)
- [ ] Add save schema migration tests for `SaveVersion` upgrades to guarantee long-term campaign compatibility. (@codex)
- [x] Build production main menu/start menu flow (new game, continue, options, quit) with clean boot routing. (@codex)
- [x] Author Opening Chapter map as first vertical slice level with encounter pacing and traversal landmarks. (@codex)
- [ ] Replace placeholder player/enemy meshes with production-ready models and animation blueprints. (@codex)
- [x] Implement quest/story objective system for chapter progression and failure/retry states. (@codex)
- [x] Add narrative intro sequence (camera, dialogue/text, mission kickoff). (@codex)
- [x] Establish asset integration pipeline (naming, folders, LODs, collision, materials) for art/audio imports. (@codex)
- [x] Implement options/settings menu (video, audio, controls) with persistent save. (@codex)
- [x] Add storefront-quality HUD/UX pass for readability and controller-first navigation. (@codex)

## Updates (2026-02-06)
- [x] Delivered Unreal level bootstrap automation and campaign map defaults (`docs/UNREAL_LEVEL_BOOTSTRAP.md`, `unreal/TheNazareneAAA/Tools/create_campaign_level.py`). (@codex)
- [x] Replaced code-only HUD with UMG pause/menu stack and in-UI save/load slot summaries (`UNazareneHUDWidget`, `ANazareneHUD`). (@codex)
- [x] Added AI Perception + Behavior Tree controller hooks (`ANazareneEnemyAIController`) and enemy integration. (@codex)
- [x] Added animation runtime state classes and retarget-ready skeletal mesh hooks for player/enemies. (@codex)
- [x] Added Niagara/audio/material integration hooks across combat systems and region environment pass. (@codex)
- [x] Published gameplay validation, packaging pipeline, GAS evaluation, and networking scope docs. (@codex)

## Updates (2026-02-13)
- [x] Fixed Unreal 5.7 startup path, default map routing, and launch blockers; project now boots into `/Game/Maps/NazareneCampaign`. (@codex)
- [x] Added startup menu overlay flow (`New Game`, `Continue`, `Options`, `Quit`) and wired menu-driven pause/input control in HUD runtime code. (@codex)
- [x] Implemented persistent player settings subsystem (controls/video/audio baseline) and connected runtime options menu controls with save/apply behavior. (@codex)
- [x] Added chapter objective progression hooks (prayer-site consecration milestone + intro story beat sequence) to campaign flow. (@codex)
- [x] Added explicit chapter stage state machine and per-region retry tracking persisted in campaign save payload. (@codex)
- [x] Added asset integration pipeline standards doc + Unreal Python validator for naming, LOD, and collision checks. (@codex)
- [x] Polished runtime HUD readability with clearer control legend, combat-state telemetry, and critical low-resource alerts. (@codex)
- [x] Added opening chapter layout automation for repeatable encounter pacing and traversal landmarks in `NazareneCampaign`. (@codex)
