# Unreal Migration Notes

## Status (February 6, 2026)
- Unreal Engine 5 C++ is the active development target.
- legacy prototype implementation is retained for feature parity reference and migration validation.
- Delivery scope should now be planned from Unreal assets, systems, and tooling.
- UMG HUD/menu stack, AI perception/controller scaffolding, animation runtime classes, and production pipeline docs are now in place.

## Source-of-truth locations
- Unreal project: `unreal/TheNazareneAAA`
- Active task list: `TODO.md`
- Legacy non-UE artifacts are retired from the active codebase.
- Execution docs:
  - `docs/UNREAL_LEVEL_BOOTSTRAP.md`
  - `docs/UNREAL_GAMEPLAY_VALIDATION.md`
  - `docs/PACKAGING_PROFILE_PIPELINE.md`
  - `docs/GAS_ADOPTION_EVALUATION.md`
  - `docs/NETWORKING_SCOPE.md`

## Prior prototype -> Unreal system mapping
- `Player character gameplay controller` -> `ANazarenePlayerCharacter`
  - Lock-on, stamina combat windows, dodge/parry/block, miracles, prayer interactions, save/load inputs.
- `Enemy combat/behavior logic` -> `ANazareneEnemyCharacter`
  - Archetypes (shield/spear/ranged/demon/boss), state transitions, parry/riposte, boss phase escalation.
- `Prayer site interaction logic` -> `ANazarenePrayerSite`
  - Overlap prompts, rest interaction, respawn anchor behavior.
- `Travel gate progression logic` -> `ANazareneTravelGate`
  - Boss-gated region transition flow.
- `Campaign/session/save orchestration` ->
  - `ANazareneCampaignGameMode`
  - `UNazareneGameInstance`
  - `UNazareneSaveSubsystem`
  - `UNazareneSaveGame`
- `HUD and prompt presentation` -> `ANazareneHUD`
  - Runtime vitals/objective/context presentation.

## Unreal project structure
- `unreal/TheNazareneAAA/TheNazareneAAA.uproject`
- `unreal/TheNazareneAAA/Config/*.ini`
- `unreal/TheNazareneAAA/Source/TheNazareneAAA`

## Current gap summary
- C++ parity baseline exists and now includes:
  - UMG pause/save/load front-end with slot summaries.
  - AI controller + perception + behavior-tree hookup points.
  - Animation runtime data classes for player/enemy animation blueprints.
  - Audio/VFX integration hooks (Niagara/SoundBase references on combat events).
  - Region-specific environment generation pass for all four campaign regions.
- Remaining gap is primarily content authoring polish:
  - Final map dressing and production-quality art assets.
  - Final animation content and retargeted sets authored in editor.
  - Final audio content (SoundCue/MetaSound) and Niagara asset tuning.
  - Runtime performance capture on target hardware with Unreal Insights.

## Immediate next execution steps
1. Run `Tools/create_campaign_level.py` in Unreal editor to generate/open `/Game/Maps/NazareneCampaign`.
2. Author final `.uasset` content (input assets, behavior tree assets, animation blueprints, Niagara/audio assets) against the new runtime hooks.
3. Execute PIE parity validation and profiling steps in `docs/UNREAL_GAMEPLAY_VALIDATION.md` and `docs/PACKAGING_PROFILE_PIPELINE.md`.
4. Close out remaining content polish and art replacement tasks per region.

## Legacy documentation policy
- `docs/PHASE1.md`, `docs/PHASE2.md`, `docs/PHASE3.md`, `docs/AAA_REVIEW.md`, and `docs/CODE_REVIEW_REPORT.md` are historical records from the pre-UE5 phase.
- Keep those documents for traceability, but plan and execute new work from Unreal docs and tasks.
