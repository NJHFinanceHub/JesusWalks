# Unreal Migration Notes

## Status (February 6, 2026)
- Unreal Engine 5 C++ is the active development target.
- Godot implementation is retained for feature parity reference and migration validation.
- Delivery scope should now be planned from Unreal assets, systems, and tooling.

## Source-of-truth locations
- Unreal project: `unreal/TheNazareneAAA`
- Active task list: `TODO.md`
- Legacy Godot reference: `project.godot`, `scenes/`, `scripts/`

## Godot -> Unreal system mapping
- `PlayerController.gd` -> `ANazarenePlayerCharacter`
  - Lock-on, stamina combat windows, dodge/parry/block, miracles, prayer interactions, save/load inputs.
- `enemy_ai.gd` -> `ANazareneEnemyCharacter`
  - Archetypes (shield/spear/ranged/demon/boss), state transitions, parry/riposte, boss phase escalation.
- `prayer_site.gd` -> `ANazarenePrayerSite`
  - Overlap prompts, rest interaction, respawn anchor behavior.
- `travel_gate.gd` -> `ANazareneTravelGate`
  - Boss-gated region transition flow.
- `campaign_manager.gd` + `game_session.gd` + `save_system.gd` ->
  - `ANazareneCampaignGameMode`
  - `UNazareneGameInstance`
  - `UNazareneSaveSubsystem`
  - `UNazareneSaveGame`
- `hud.gd` -> `ANazareneHUD`
  - Runtime vitals/objective/context presentation.

## Unreal project structure
- `unreal/TheNazareneAAA/TheNazareneAAA.uproject`
- `unreal/TheNazareneAAA/Config/*.ini`
- `unreal/TheNazareneAAA/Source/TheNazareneAAA`

## Current gap summary
- C++ gameplay parity baseline exists.
- Authoring-heavy production content is still pending:
  - Maps (`.umap`) and editor-authored `.uasset` content.
  - Animation blueprints, finalized skeletal assets, and combat animation polish.
  - UMG/CommonUI menus and front-end save/load UX.
  - Niagara, materials, SoundCue/MetaSound integration.
  - Performance tuning and packaging pipeline hardening.

## Immediate next execution steps
1. Stand up a playable Unreal campaign map and validate the full loop in PIE.
2. Move input to Enhanced Input assets.
3. Replace code-only HUD/menu flow with UMG/CommonUI.
4. Shift enemy logic into Behavior Trees + AI Perception where appropriate.
5. Run parity validation against Godot reference behavior and document gaps.

## Legacy documentation policy
- `docs/PHASE1.md`, `docs/PHASE2.md`, `docs/PHASE3.md`, `docs/AAA_REVIEW.md`, and `docs/CODE_REVIEW_REPORT.md` are historical records of Godot work.
- Keep those documents for traceability, but plan and execute new work from Unreal docs and tasks.
