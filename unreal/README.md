# The Nazarene AAA (Unreal Migration)

This folder contains the Unreal Engine 5.4 project that mirrors the current Godot prototype gameplay systems.

## Implemented gameplay stack (current)

| Layer | Unreal Systems | Scope |
| --- | --- | --- |
| Game bootstrap | `UNazareneGameInstance`, `ANazareneGameMode` | Startup classes and region defaults. |
| Player combat + miracles | `ANazareneHeroCharacter`, `FNazareneMiracle` | Health/faith pools, melee, miracle cooldown + cost logic. |
| Enemy combat model | `ANazareneEnemyCharacter`, `FNazareneEnemyArchetype` | Archetype-driven stats + runtime damage handling. |
| Campaign progression | `UNazareneCampaignSubsystem`, `FNazareneCampaignState` | Prayer tracking, region wins, chapter progression, soul rewards. |
| Persistence | `UNazareneSaveGame`, `UNazareneSaveSubsystem` | Slot-based save/load for hero stats, location, campaign state. |

## How the Godot prototype maps to Unreal

| Godot System | Unreal Counterpart |
| --- | --- |
| Player combat + miracles | `ANazareneHeroCharacter`, `FNazareneMiracle` |
| Enemy archetypes | `ANazareneEnemyCharacter`, `FNazareneEnemyArchetype` |
| Campaign flow / regions | `UNazareneCampaignSubsystem`, `FNazareneCampaignState` |
| Rest + save | `UNazareneSaveGame`, `UNazareneSaveSubsystem` |
| Game state boot | `UNazareneGameInstance`, `ANazareneGameMode` |

## Next steps for production

1. Create map assets under `Content/Maps` and update `DefaultEngine.ini` to point at the real startup map.
2. Build UI in UMG for campaign, prayer, and rest menus.
3. Add AI behavior trees and EQS for enemy archetypes.
4. Drive miracles, enemies, and regions from data assets and data tables.
5. Add online services (leaderboards/cloud-save/co-op session flow) as backend endpoints stabilize.

## Build notes

Open `unreal/TheNazareneAAA/TheNazareneAAA.uproject` with Unreal Engine 5.4 and generate project files for your platform.
