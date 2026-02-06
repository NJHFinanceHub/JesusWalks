# Unreal Migration Notes

## Scope delivered in this repo
The Unreal C++ project is under `unreal/TheNazareneAAA` and mirrors the currently implemented Godot systems:

- `PlayerController.gd` -> `ANazarenePlayerCharacter`:
  - lock-on, stamina combat windows, dodge/parry/block, miracles (heal/blessing/radiance)
  - prayer interaction, slot save/load input, defeat -> prayer-site respawn
- `enemy_ai.gd` -> `ANazareneEnemyCharacter`:
  - archetypes (shield/spear/ranged/demon/boss), state machine, parry/riposte, phase scaling
- `prayer_site.gd` -> `ANazarenePrayerSite`:
  - overlap-driven prompt binding and rest target
- `travel_gate.gd` -> `ANazareneTravelGate`:
  - boss-gated region transitions
- `campaign_manager.gd` + `game_session.gd` + `save_system.gd` ->
  - `ANazareneCampaignGameMode`
  - `UNazareneGameInstance`
  - `UNazareneSaveSubsystem`
  - `UNazareneSaveGame`
- `hud.gd` -> `ANazareneHUD` (runtime canvas HUD)

## Unreal project structure
- `unreal/TheNazareneAAA/TheNazareneAAA.uproject`
- `unreal/TheNazareneAAA/Config/*.ini`
- `unreal/TheNazareneAAA/Source/TheNazareneAAA`

## What still needs editor-side authoring
This commit provides gameplay code and config, but no `.umap` or `.uasset` content authored in the Unreal editor yet.

Minimum editor steps:
1. Right-click `TheNazareneAAA.uproject` and generate project files.
2. Build the C++ target in Visual Studio.
3. Open the project in Unreal Editor 5.4+.
4. Create a map (or use an existing map) and set `NazareneCampaignGameMode` if not already defaulted.
5. Iterate on animation blueprints, skeletal meshes, Niagara, audio assets, and UI widget assets for production polish.

## AAA production next milestones
- Replace primitive meshes with production character/enemy/environment assets.
- Move input to Enhanced Input assets and action mapping contexts.
- Convert HUD to UMG/CommonUI widget stack and platform navigation.
- Replace direct tick-based AI with Behavior Trees + EQS + AI Perception.
- Integrate GAS (Gameplay Ability System) for abilities, cooldowns, and status effects.
- Add netcode architecture decisions early (single-player only vs co-op).
