Status: Active

# Repository Status Audit (2026-02-14)

## Scope
This snapshot records repository/documentation/task state before startup/visual stability execution.

## Baseline
- Date: 2026-02-14
- Branch: `main`
- HEAD: `3797be0`
- Remote tracking: `origin/main`

## Git Working Tree
- Tracked modifications at audit time: docs status banners, gameplay/GAS fixes already in progress.
- Untracked directory: `unreal/Packaged/`

## TODO Snapshot
- Source: `TODO.md`
- Checked: `31`
- Open: `25`

## Document Inventory
- Total markdown files: `29`
- Files under `docs/`: `23`
- Governance policy applied: every markdown file has `Status: Active` or `Status: Historical`.

## Confirmed Hard Blockers
- `Content/Art/VFX` directory: **missing**
- Region music assets in `Content/Audio/Music`: **4 assets present**
- `NazareneLoreItem` runtime class: **missing**

## Runtime Asset Inventory (Evidence)
### Region maps
- `unreal/TheNazareneAAA/Content/Maps/Regions/Galilee/L_GalileeShores.umap`
- `unreal/TheNazareneAAA/Content/Maps/Regions/Decapolis/L_DecapolisRuins.umap`
- `unreal/TheNazareneAAA/Content/Maps/Regions/Wilderness/L_WildernessTemptation.umap`
- `unreal/TheNazareneAAA/Content/Maps/Regions/Jerusalem/L_JerusalemApproach.umap`
- `unreal/TheNazareneAAA/Content/Maps/Regions/Gethsemane/L_GardenGethsemane.umap`
- `unreal/TheNazareneAAA/Content/Maps/Regions/ViaDolorosa/L_ViaDolorosa.umap`
- `unreal/TheNazareneAAA/Content/Maps/Regions/EmptyTomb/L_EmptyTomb.umap`

### Music assets
- `unreal/TheNazareneAAA/Content/Audio/Music/S_Music_Galilee.uasset`
- `unreal/TheNazareneAAA/Content/Audio/Music/S_Music_Decapolis.uasset`
- `unreal/TheNazareneAAA/Content/Audio/Music/S_Music_Wilderness.uasset`
- `unreal/TheNazareneAAA/Content/Audio/Music/S_Music_Jerusalem.uasset`

### Character assets
- `unreal/TheNazareneAAA/Content/Art/Characters/Player/SK_BiblicalHero.uasset`
- `unreal/TheNazareneAAA/Content/Art/Characters/Enemies/SK_BiblicalLegionary.uasset`
- `unreal/TheNazareneAAA/Content/Art/Characters/Common/SK_BiblicalHumanoid.uasset`
- `unreal/TheNazareneAAA/Content/Art/Characters/Common/SKEL_BiblicalHumanoid.uasset`
- `unreal/TheNazareneAAA/Content/Art/Characters/Common/PHYS_BiblicalHumanoid.uasset`

### Behavior trees
- `unreal/TheNazareneAAA/Content/AI/BehaviorTrees/BT_MeleeShield.uasset`
- `unreal/TheNazareneAAA/Content/AI/BehaviorTrees/BT_Spear.uasset`
- `unreal/TheNazareneAAA/Content/AI/BehaviorTrees/BT_Ranged.uasset`
- `unreal/TheNazareneAAA/Content/AI/BehaviorTrees/BT_Demon.uasset`
- `unreal/TheNazareneAAA/Content/AI/BehaviorTrees/BT_Boss.uasset`

## Notes
- This audit captures repository truth only; runtime validation evidence is tracked separately in subsequent execution updates.

## Runtime Validation Addendum (2026-02-14)
- Build verification: `Build.bat TheNazareneAAAEditor Win64 Development` succeeded after startup/menu fixes.
- Packaging verification: `RunUAT BuildCookRun ... -build -cook -allmaps -stage -pak -archive` succeeded.
- Packaging contract fix applied:
  - `unreal/TheNazareneAAA/Config/DefaultGame.ini` now contains explicit `+MapsToCook` entries for all region sublevels.
  - `DirectoriesToAlwaysCook` retained for `/Game/Maps`, `/Game/AI`, `/Game/Art`, `/Game/Audio`.
- Runtime packaged launch verification (`unreal/Packaged/Windows/TheNazareneAAA.exe`):
  - No global shader library initialization failure.
  - No startup missing-map failure.
  - `Region sublevel missing` warning cleared after explicit map cook entries.
  - Start menu remains visible with gameplay HUD gated until Start/Continue.
- Remaining known content gap:
  - Missing Niagara assets under `/Game/Art/VFX/*` still log unresolved references when those effects are requested.
