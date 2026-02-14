Status: Active

# The Nazarene AAA (Unreal Engine)

This is the primary runtime codebase for The Nazarene.
This repository is now UE5-first; all active gameplay work must stay in UE5-compatible language/tooling.

## Build
1. Install Unreal Engine 5.4+ and Visual Studio with C++ toolchain.
2. Open `unreal/TheNazareneAAA/TheNazareneAAA.uproject`.
3. If prompted, generate project files and build.
4. Launch in editor and run PIE.

## Implemented systems
- `ANazareneCampaignGameMode`: region flow, boss-gated progression, reward unlocks, checkpointing.
- `ANazarenePlayerCharacter`: movement/combat/miracle/parry/block/lock-on plus enhanced-input mapping context flow and prayer/save/load interactions.
- `ANazareneEnemyCharacter` + `ANazareneEnemyAIController`: archetype AI state machine with AI Perception and behavior-tree integration points.
- `ANazarenePrayerSite` and `ANazareneTravelGate`: interaction anchors.
- `UNazareneSaveSubsystem` + `UNazareneSaveGame`: slot and checkpoint payload persistence.
- `ANazareneHUD` + `UNazareneHUDWidget`: UMG HUD/pause menu stack with in-UI slot summaries and save/load actions.
- `UNazarenePlayerAnimInstance` and `UNazareneEnemyAnimInstance`: animation blueprint runtime state feeds.
- Niagara/audio presentation hooks across combat and miracle events.

## Notes
- This migration intentionally uses code-first systems so gameplay parity is reproducible before asset-heavy production.
- Map bootstrap helper: `unreal/TheNazareneAAA/Tools/create_campaign_level.py`.
- Final AAA quality still requires authored maps, skeletal animation sets, materials, VFX, audio, cinematic pipeline, and optimization passes.
