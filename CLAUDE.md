# CLAUDE.md — The Nazarene

> AI assistant guide for the **The Nazarene** biblical action RPG built in Unreal Engine 5.7 C++.

## Project Overview

**The Nazarene** is a single-player biblical action RPG following a 7-chapter campaign from Jesus's birth through resurrection. The codebase is a fully migrated UE5 C++ project (from a pre-UE5 prototype). Core gameplay systems are functionally complete; the project is in a production art/content/polish phase targeting a Win64 Shipping release.

- **Engine:** Unreal Engine 5.7
- **Language:** C++ (UE5 conventions), Python tooling
- **Platform:** Win64 Shipping
- **Developer:** Solo (NJHFinanceHub)
- **Scope:** Single-player only (no multiplayer)

## Repository Structure

```
JesusWalks/
├── README.md                      # Quick start guide
├── TODO.md                        # 8-week storefront-ready production roadmap (source of truth for task status)
├── CLAUDE.md                      # This file
├── icon.svg                       # Project icon
├── assets/music/                  # Raw audio assets
├── docs/                          # 24 design/architecture documents
│   ├── PRODUCTION_ROADMAP.md      # High-level milestones
│   ├── PLOT.md                    # Gospel-aligned campaign narrative
│   ├── IMPLEMENTATION_PLAN.md     # Systems design decisions
│   ├── CHARACTER_LIBRARY.md       # Character lore and archetypes
│   ├── GAS_ADOPTION_EVALUATION.md # GameplayAbilities adoption plan
│   ├── PACKAGING_PROFILE_PIPELINE.md  # Build & ship pipeline
│   └── ...                        # See README.md § Documentation map
└── unreal/TheNazareneAAA/         # *** PRIMARY PROJECT ***
    ├── TheNazareneAAA.uproject
    ├── Config/
    │   ├── DefaultEngine.ini
    │   ├── DefaultGame.ini
    │   ├── DefaultInput.ini
    │   └── NazareneAssetOverrides.ini  # Asset path mappings
    ├── Source/TheNazareneAAA/
    │   ├── Public/   (33 headers)
    │   ├── Private/  (31 implementations)
    │   └── TheNazareneAAA.Build.cs
    ├── Content/
    │   ├── AI/BehaviorTrees/, AI/Blackboards/
    │   ├── Art/Animation/, Art/Characters/, Art/Environment/, Art/Materials/
    │   ├── Audio/Music/ (7 regions), Audio/SFX/
    │   └── Maps/NazareneCampaign, Maps/Regions/{Galilee,Decapolis,Wilderness,Jerusalem,Gethsemane,ViaDolorosa,EmptyTomb}/
    └── Tools/                     # 13 Python automation scripts
```

## Key Source Files

| File | Purpose |
|------|---------|
| `NazareneCampaignGameMode.h/.cpp` | Central campaign orchestration: region loading, progression, music state |
| `NazarenePlayerCharacter.h/.cpp` | Player: movement, combat (light/heavy), miracles, dodge/parry/block, lock-on |
| `NazareneEnemyCharacter.h/.cpp` | Enemy: 5 archetypes, 11-state AI state machine |
| `NazareneTypes.h` | All shared data structures, enums, and type definitions |
| `NazareneAbilitySystemComponent.h/.cpp` | GAS component (Phase 1) |
| `NazareneAttributeSet.h/.cpp` | GAS attributes: Health, Stamina, Faith |
| `NazareneSaveSubsystem.h/.cpp` | Slot-based save/load with checkpoint snapshots |
| `NazareneHUD.h/.cpp` + `NazareneHUDWidget.h/.cpp` | Full UMG HUD: pause menu, save/load, damage numbers, health bars |
| `NazareneSkillTree.h/.cpp` | 4-branch skill tree (Combat, Movement, Miracles, Defense) |
| `NazareneRegionDataAsset.h/.cpp` | Data-driven region definitions |
| `NazareneSettingsSubsystem.h/.cpp` | Persistent player settings (controls, video, audio, accessibility) |
| `NazareneEnemyAIController.h/.cpp` | AI Perception + Behavior Tree controller |
| `NazareneAssetResolver.h/.cpp` | Runtime asset fallback resolver for missing content |
| `NazareneVFXSubsystem.h/.cpp` | Centralized VFX management |
| `TheNazareneAAA.Build.cs` | Module dependencies and build config |

## Architecture & Core Systems

### Campaign Flow
`ANazareneCampaignGameMode` drives the 7-chapter campaign. Each chapter follows a stage state machine:
**ConsecratePrayerSite → DefeatGuardian → ReachTravelGate → Completed**

Region data is defined via `FNazareneRegionDefinition` structs and `UNazareneRegionDataAsset`. Music state transitions between Peace, Tension, Combat, Boss, and Victory.

### Combat System
The player (`ANazarenePlayerCharacter`) supports light/heavy attacks, dodge, parry, block, lock-on targeting, and miracle abilities. Enemies (`ANazareneEnemyCharacter`) use 5 archetypes (MeleeShield, Spear, Ranged, Demon, Boss) with an 11-state machine (Idle, Chase, Retreat, Strafe, Windup, Casting, Blocking, Recover, Staggered, Parried, Redeemed).

### GameplayAbilities (GAS)
Phase 1 is implemented: `UNazareneAbilitySystemComponent`, `UNazareneAttributeSet` (Health, MaxHealth, Stamina, MaxStamina, Faith), and 3 miracle abilities (`UGA_NazareneHeal`, `UGA_NazareneBlessing`, `UGA_NazareneRadiance`). Phase 2 (full GAS with GameplayEffects) is planned.

### Save/Load
`UNazareneSaveSubsystem` provides slot-based saves with `FNazareneSavePayload` containing player snapshots, enemy snapshots, and full campaign state. Checkpoint-aware with per-region retry tracking.

### UI/UX
`ANazareneHUD` + `UNazareneHUDWidget` manage the full UMG widget stack: pause menu, save/load slots, damage numbers, enemy health bars, death overlay, loading screen with lore tips, and skill tree access. Controller-first UX via CommonUI.

## Naming Conventions

All project classes use the `Nazarene` prefix:

| Type | Pattern | Examples |
|------|---------|----------|
| Actors | `ANazarene*` | `ANazarenePlayerCharacter`, `ANazarenePrayerSite` |
| UObjects/Components | `UNazarene*` | `UNazareneSaveSubsystem`, `UNazareneSkillTree` |
| Abilities | `UGA_Nazarene*` | `UGA_NazareneHeal`, `UGA_NazareneRadiance` |
| Anim Instances | `UNazarene*AnimInstance` | `UNazarenePlayerAnimInstance` |
| Widgets | `UNazarene*Widget` | `UNazareneHUDWidget`, `UNazareneDamageNumberWidget` |
| Structs | `FNazarene*` | `FNazarenePlayerSnapshot`, `FNazareneCampaignState` |
| Enums | `ENazarene*` | `ENazareneEnemyArchetype`, `ENazareneEnemyState` |
| Data Assets | `UNazarene*DataAsset` | `UNazareneRegionDataAsset` |

## Coding Conventions

- **UE5 reflection macros**: All gameplay-exposed properties use `UPROPERTY(EditAnywhere, BlueprintReadWrite)` with category grouping (Vitals, Movement, Campaign, Audio, etc.)
- **Blueprint exposure**: Functions use `UFUNCTION(BlueprintCallable, Category = "...")` routing
- **Soft references**: Asset references use `TSoftObjectPtr<>` for streaming
- **Component architecture**: Standard Spring Arm + Camera, Capsule + CharacterMovement pattern
- **Input system**: Enhanced Input with mapping contexts (`UInputMappingContext`, `UInputAction`)
- **PCH mode**: `UseExplicitOrSharedPCHs`
- **Attribute macros**: Custom `NAZARENE_ATTRIBUTE_ACCESSORS` helper for GAS attributes
- **Header/Implementation split**: Public/ for .h, Private/ for .cpp — standard UE5 module layout
- **Enum values**: Explicitly numbered with `UMETA(DisplayName)` annotations
- **Default initialization**: All struct members have default values

## Module Dependencies

**Public:** Core, CoreUObject, Engine, InputCore, EnhancedInput, UMG, AIModule, GameplayAbilities, GameplayTags, GameplayTasks, Niagara

**Private:** Slate, SlateCore

**Plugins:** EnhancedInput, Niagara, CommonUI, GameplayAbilities

## Build & Run

### Quick Start
1. Install Unreal Engine 5.7+ and Visual Studio with C++ toolchain
2. Open `unreal/TheNazareneAAA/TheNazareneAAA.uproject`
3. Generate project files if prompted, then build
4. Launch in Unreal Editor and run PIE (Play In Editor)

### Packaging (Win64 Shipping)
Follow `docs/PACKAGING_PROFILE_PIPELINE.md` for UAT `BuildCookRun` commands and PDB archiving.

### Python Tools
Located in `unreal/TheNazareneAAA/Tools/`. Run via Unreal Editor Python:
```
UnrealEditor-Cmd.exe -run=pythonscript -script=<script.py> -unattended -nop4
```

Key tools:
- `validate_asset_pipeline.py` — Asset naming, LOD, and collision checks
- `validate_vertical_slice_content.py` — Map/mesh/music coverage validation
- `create_region_sublevels.py` — Bootstrap region streaming levels
- `run_galilee_pie_soak.py` — PIE soak test automation

## Testing & Validation

There is no automated CI/CD pipeline. Validation is manual per the roadmap:

- **PIE soak tests**: 20-minute playthroughs per region to catch crashes/soft-locks
- **Performance profiling**: Unreal Insights (`-trace=cpu,gpu,frame`)
- **Performance budgets**: ≤16.6ms frame time, ≤2000 draw calls, ≤3.5GB VRAM
- **Save/load regression**: 7 regions × 3 slots × checkpoint matrix
- **Input validation**: Every action on gamepad + KBM including edge cases
- **Crash soak**: 60-minute session in Development then Test config

## Content Structure (7 Campaign Chapters)

| # | Region | Theme |
|---|--------|-------|
| 1 | Galilee Shores | Birth, early ministry, lakeside |
| 2 | Decapolis Ruins | Greco-Roman ruins, overcast |
| 3 | Wilderness of Temptation | Desert canyons, harsh sun |
| 4 | Jerusalem Approach | City walls, golden hour |
| 5 | Garden of Gethsemane | Olive grove, moonlit betrayal |
| 6 | Via Dolorosa | Narrow streets, trial |
| 7 | The Empty Tomb | Cave, dawn, resurrection |

Maps live under `Content/Maps/Regions/<RegionName>/`. The main persistent level is `Content/Maps/NazareneCampaign`.

## Current Status & Roadmap

See `TODO.md` for the authoritative 8-week production roadmap. As of February 2026:

- **Completed**: Full C++ scaffold, GAS Phase 1, skill tree, save/load, HUD, animation instances, asset resolver, settings subsystem, Enhanced Input with gamepad, Python tooling
- **In progress**: Week 1 — asset pipeline bootstrap, character meshes imported, first PIE soak pending
- **Upcoming**: Environment art, BT authoring, GAS Phase 2 miracles, Niagara VFX, UI polish, QA, packaging

## Important Docs for Context

| Document | What it covers |
|----------|---------------|
| `TODO.md` | Week-by-week task list with checkboxes — check this first |
| `docs/PLOT.md` | Full campaign narrative (Gospel-aligned) |
| `docs/IMPLEMENTATION_PLAN.md` | Architecture decisions and trade-offs |
| `docs/GAS_ADOPTION_EVALUATION.md` | GAS Phase 1 vs Phase 2 analysis |
| `docs/AAA_ARCHITECTURE_FRAMEWORK_REVIEW.md` | Architectural gaps and recommendations |
| `docs/PACKAGING_PROFILE_PIPELINE.md` | Win64 build/ship commands |
| `docs/ASSET_INTEGRATION_PIPELINE.md` | Asset standards and validation |
| `docs/CHARACTER_LIBRARY.md` | Character lore and archetypes |

## Guidelines for AI Assistants

1. **Read before editing**: Always read source files before modifying them. Understand existing patterns.
2. **Follow naming conventions**: All new classes must use the `Nazarene` prefix patterns documented above.
3. **Check TODO.md first**: The roadmap defines what work is needed and what's already done.
4. **Respect the architecture**: The codebase uses component-based UE5 patterns with GAS. Don't introduce new architectural patterns without justification.
5. **No multiplayer**: This is single-player only (`docs/NETWORKING_SCOPE.md`). Don't add networking features.
6. **Header/Implementation split**: Headers in `Public/`, implementations in `Private/`. Follow the existing module layout.
7. **Data-driven design**: Prefer `UDataAsset`, `UDataTable`, and config over hard-coded values.
8. **Asset paths**: Check `Config/NazareneAssetOverrides.ini` for canonical asset path mappings.
9. **Content directory layout**: Follow existing `Content/` folder hierarchy (Art/, Audio/, Maps/, AI/).
10. **Don't delete docs/**: The `docs/` folder contains critical design documents that inform implementation decisions.
11. **Python tools**: Automation scripts in `Tools/` are run inside the Unreal Editor Python environment, not standalone Python.
12. **Git**: The `.gitignore` excludes Binaries/, Build/, DerivedDataCache/, Intermediate/, Saved/, .vs/, .sln files. Never commit these.
13. **Blueprint exposure**: Mark properties and functions for Blueprint access where appropriate for designer iteration.
14. **Theological accuracy**: Campaign narrative follows Gospel chronology. Consult `docs/PLOT.md` and `docs/CHARACTER_LIBRARY.md` for canonical character/story references.
