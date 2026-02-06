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
- [ ] Create initial playable `.umap` campaign level and set `NazareneCampaignGameMode` defaults. (@codex)
- [ ] Replace direct input bindings with Enhanced Input assets and mapping contexts. (@claude)
- [ ] Build UMG/CommonUI HUD + menu stack to replace code-only HUD flow. (@codex)
- [ ] Implement Behavior Tree + AI Perception pipeline for enemy decision logic. (@claude)
- [ ] Author Animation Blueprint state machines and retargeted skeletal animation sets for player and enemies. (@codex)
- [ ] Integrate combat/UI/audio with Unreal assets (SoundCues/MetaSounds, Niagara, materials). (@claude)
- [ ] Add save/load front-end menu flow and slot summaries in Unreal UI. (@codex)
- [ ] Run Unreal gameplay validation pass and document balance tuning against migration parity targets. (@claude)
- [ ] Define packaging/profile pipeline (Win64 target, perf budgets, crash logs). (@codex)

## Backlog (after parity)
- [ ] Evaluate GAS adoption for miracles, cooldowns, buffs, and status effects. (@claude)
- [ ] Define networking scope (single-player only vs co-op) and technical implications. (@codex)
- [ ] Replace placeholder geometry with production environments for all four regions. (@claude)
