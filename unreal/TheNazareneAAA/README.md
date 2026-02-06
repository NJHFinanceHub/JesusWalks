# The Nazarene AAA (Unreal Engine)

This is the primary runtime codebase for The Nazarene.
The Godot project remains in the repository as legacy reference material for migration parity checks.

## Build
1. Install Unreal Engine 5.4+ and Visual Studio with C++ toolchain.
2. Open `unreal/TheNazareneAAA/TheNazareneAAA.uproject`.
3. If prompted, generate project files and build.
4. Launch in editor and run PIE.

## Implemented systems
- `ANazareneCampaignGameMode`: region flow, boss-gated progression, reward unlocks, checkpointing.
- `ANazarenePlayerCharacter`: movement/combat/miracle/parry/block/lock-on plus prayer and save-load inputs.
- `ANazareneEnemyCharacter`: archetype AI, windup/strike/recovery timing, parry/riposte, boss phase escalation.
- `ANazarenePrayerSite` and `ANazareneTravelGate`: interaction anchors.
- `UNazareneSaveSubsystem` + `UNazareneSaveGame`: slot and checkpoint payload persistence.
- `ANazareneHUD`: runtime gameplay HUD overlay.

## Notes
- This migration intentionally uses code-first systems so gameplay parity is reproducible before asset-heavy production.
- Final AAA quality still requires authored maps, skeletal animation sets, materials, VFX, audio, cinematic pipeline, and optimization passes.
