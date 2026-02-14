# The Nazarene â€” Storefront-Ready Production Roadmap

> **Last updated:** February 14, 2026
> **Engine:** Unreal Engine 5.7 C++ (`unreal/TheNazareneAAA`)
> **Target:** Win64 Shipping build â€” 7-chapter campaign from Nativity to Crucifixion, production art/audio, polished UI
> **Developer:** Solo

## Status Summary

The Unreal C++ codebase is **functionally complete** for core gameplay (combat, miracles, AI, campaign, save/load, HUD, settings) but runs entirely on placeholder geometry. The `Content/` directory is nearly empty â€” no skeletal meshes, no AnimBlueprints, no Behavior Tree assets, no Niagara VFX, no audio assets, and no authored environments. All former non-UE gameplay logic has been migrated into UE5-compatible systems.

### Completed Baseline
- Unreal C++ scaffold mirrors all major prior gameplay systems
- `ANazareneCampaignGameMode`, `ANazarenePlayerCharacter`, `ANazareneEnemyCharacter`
- `ANazarenePrayerSite`, `ANazareneTravelGate`, `UNazareneSaveSubsystem`
- `ANazareneHUD` with UMG pause/menu stack and save/load slot summaries
- AI Perception + Behavior Tree controller hooks (`ANazareneEnemyAIController`)
- Animation runtime state classes and retarget-ready skeletal mesh hooks
- Enhanced Input with mapping contexts
- Persistent player settings subsystem (controls/video/audio)
- Chapter objective progression, stage state machine, retry tracking
- Asset integration pipeline standards + Python validator
- Packaging/profile pipeline docs (`docs/PACKAGING_PROFILE_PIPELINE.md`)
- GAS evaluation completed (`docs/GAS_ADOPTION_EVALUATION.md`)
- Networking scope defined (single-player only)
- Pre-UE5 prototype audit completed and issues addressed (Feb 13, 2026)

---

## Week 1 â€” Asset Pipeline Bootstrap + Character Meshes

> Highest-risk items first: marketplace skeleton retargeting determines whether animation work can proceed.

- [x] Acquire and import player skeletal mesh to `Content/Art/Characters/Player/`; configure `ProductionSkeletalMesh` soft pointer on `ANazarenePlayerCharacter`
- [x] Author `ABP_Player` AnimBlueprint wired to `UNazarenePlayerAnimInstance` (Idle/Locomotion/Attack/Dodge/Block/Hurt/Defeated states)
- [x] Acquire and import enemy skeletal mesh (one base rig, archetype variants via material/prop swaps) to `Content/Art/Characters/Enemies/`
- [x] Author `ABP_Enemy` AnimBlueprint wired to `UNazareneEnemyAnimInstance` (11 states from `ENazareneEnemyState`)
- [x] Add `GameplayAbilities` module to `Build.cs`; create stub `NazareneAttributeSet` and `NazareneAbilitySystemComponent` classes (GAS Phase 1 â€” non-breaking)
- [x] Add gamepad bindings to Enhanced Input (left stick move, right stick look, face buttons for combat/miracles, triggers for block/heavy)
- [x] Run first PIE soak test (20 min on Galilee) to validate mesh fallback paths and identify crashes

## Week 2 â€” Environment Art + Audio Pipeline + First BT

> Replace procedural BasicShapes geometry with authored environments. Stand up the audio pipeline.

- [x] Acquire modular Ancient/Mediterranean environment art kit; validate with `Tools/validate_asset_pipeline.py`
- [x] Author Galilee Shores production sublevel (`/Game/Maps/Regions/Galilee/`); refactor `LoadRegion()` to use level streaming instead of `SpawnRegionEnvironment()`
- [x] First lighting/post-processing pass on Galilee (directional light, skylight, volumetric fog, color grading)
- [x] Import combat SFX (attack whoosh, impact, dodge, hurt grunt, parry clang, miracle shimmer); assign to player/enemy `UPROPERTY` audio slots
- [x] Import `epic_biblical_theme.wav` as `S_Music_Galilee`; add `UAudioComponent` to `LoadRegion()` for region music playback
- [x] Author `BT_MeleeShield` behavior tree using existing blackboard keys (`TargetActor`, `TargetDistance`)

## Week 3 â€” Gameplay Systems Port + UI Widgets

> Complete migration of former prototype-only systems (skill tree, XP, damage numbers, health bars) into Unreal C++.

- [x] Create `UNazareneSkillTree` class mirroring prior prototype skill tree logic (4 branches, 8 skills, unlock logic); add `UnlockedSkills`/`SkillPoints` to `FNazareneCampaignState` in `NazareneTypes.h`
- [x] Implement XP/leveling: add `TotalXP`/`PlayerLevel` to campaign state; award XP in `HandleEnemyRedeemed()`; threshold formula migrated into UE5 leveling helpers
- [x] Implement `UNazareneDamageNumberWidget` (Normal/Critical/Heal/PoiseBreak/Blocked types, world-to-screen projection, float + fade)
- [x] Implement `UNazareneEnemyHealthBarWidget` (name + health + poise bars, lock-on show, auto-fade)
- [x] Implement death/respawn overlay ("You Were Struck Down" + retry count + "Rise Again" button)
- [x] Implement loading screen widget with randomized lore tips from `CHARACTER_LIBRARY.md`
- [x] Author remaining BT assets: `BT_Spear`, `BT_Ranged`, `BT_Demon`, `BT_Boss`

## Week 4 â€” Content Expansion (Chapters 5â€“7) + Region Art

> Expand the campaign from 4 to 7 chapters per the Gospel narrative arc in `docs/PLOT.md`.

- [x] Migrate region definitions from hard-coded `BuildDefaultRegions()` to DataTable/DataAsset for all 7 chapters
- [x] Define Chapter 5 "Journey to Jerusalem" (roadside villages, discipleship teachings), Chapter 6 "Passion Week" (upper room, Gethsemane, trial), Chapter 7 "Golgotha Crucifixion" (hilltop, solemn crucifixion and burial)
- [x] Author Decapolis Ruins production sublevel (ruined Greco-Roman architecture, overcast lighting)
- [x] Author Wilderness of Temptation production sublevel (desert canyons, harsh sun, heat haze)
- [x] Author Jerusalem Approach production sublevel (city walls, temple columns, golden hour)
- [x] Blockout sublevels for chapters 5â€“7 (geometry + lighting placeholder)
- [x] Add chapter-title HUD labels and per-region reward items that persist into campaign inventory
- [x] Implement `ANazareneNPC` actor with overlap interaction and dialogue text display; place 2â€“3 NPCs per region

## Week 5 â€” GAS Miracles + Niagara VFX + Boss Polish

> Convert miracles to Gameplay Abilities. Add all visual effects. Polish boss encounters.

- [x] Convert Heal/Blessing/Radiance to `UGameplayAbility` subclasses with `UGameplayEffect` cooldowns and faith costs (GAS Phase 2)
- [ ] Author miracle Niagara systems: `NS_Heal` (golden particles), `NS_Blessing` (radiant aura), `NS_Radiance` (AoE burst)
- [ ] Author combat Niagara systems: `NS_LightAttackHit`, `NS_HeavyAttackHit`, `NS_DodgeTrail`, `NS_EnemyRedeemed`; assign to existing VFX `UPROPERTY` slots
- [ ] Author ambient Niagara per region (dust motes, sand wisps, embers, light shafts, olive leaves, crowd dust, dawn rays)
- [ ] Enhance boss fights: phase-specific attack patterns, arena hazard VFX, victory cutscene trigger
- [x] Add prayer site variety (visual themes per region) and rest-time features (Faith refill, skill tree access, lore text)
- [ ] Design unique encounter compositions per chapter (escalating archetype mix, ambush spawns, reinforcement waves)

## Week 6 â€” UI/UX Polish + Audio Completion + Accessibility

> Controller-first UX. Full audio. Accessibility. Skill tree UI.

- [ ] Full gamepad navigation audit: all menus navigable with analog stick + face buttons via CommonUI focus chain
- [ ] Implement `UNazareneSkillTreeWidget` (4-branch visual display, unlock interaction, accessible from pause menu and prayer sites)
- [ ] Main menu cinematic backdrop (slow camera pan over Galilee environment with atmospheric lighting)
- [ ] Complete SFX: UI sounds (navigate, confirm, cancel, save), footsteps (surface-aware), ambient soundscapes per region
- [ ] Author/import music for all 7 regions (ambient + combat track each); implement crossfade on enemy detection
- [ ] Add accessibility settings to `FNazarenePlayerSettings`: subtitle size, colorblind mode, high-contrast HUD
- [ ] HUD polish pass: animated health/stamina/faith bars with smooth lerp, low-resource pulse, miracle cooldown radials

## Week 7 â€” QA, Performance, and Content Lock

> No new features. Stability, performance, and regression testing only.

- [ ] PIE soak test: 20-minute playthrough per region (7 regions), document and fix all crashes/soft-locks
- [ ] Performance profiling with Unreal Insights (`-trace=cpu,gpu,frame`); meet budgets: <=16.6ms frame, <=2000 draw calls, <=3.5GB VRAM
- [ ] Save/load regression matrix: 7 regions x 3 slots x checkpoint; verify campaign state, skill tree, XP persistence
- [ ] Input validation matrix: every action on gamepad + KBM; edge cases (simultaneous inputs, rapid menu toggle)
- [ ] 60-minute crash soak test (Development config then Test config)
- [ ] Final material/shader pass: PBR materials for all environment and character assets, terrain blending, water shader
- [ ] Place environmental storytelling lore items (3â€“5 `ANazareneLoreItem` per region with biblical text)

## Week 8 â€” Packaging + Store Assets + Release Candidate

> Ship it. No new features. Packaging, marketing assets, and final validation only.

- [ ] Win64 Shipping build via UAT (`BuildCookRun` command from `docs/PACKAGING_PROFILE_PIPELINE.md`); archive PDBs
- [ ] Test clean install on machine without UE5; verify redistributable prerequisites
- [ ] Capture 5+ store screenshots (combat, miracles, boss fight, environment vista, skill tree)
- [ ] Capture 60â€“90s gameplay trailer at 60fps
- [ ] Write store page description, genre tags, system requirements, feature list
- [ ] Full campaign playthrough in Shipping build (not PIE) â€” boot flow through credits
- [ ] Author credits screen and end-game flow (Chapter 7 completion -> "Pilgrimage Complete" -> credits -> main menu / NG+)

---

## New Classes to Create

| File | Purpose |
|------|---------|
| `NazareneSkillTree.h/.cpp` | Skill tree data and logic implemented in UE5 runtime |
| `NazareneAttributeSet.h/.cpp` | GAS attribute set for health/stamina/faith |
| `NazareneAbilitySystemComponent.h/.cpp` | Player Ability System Component |
| `GA_Heal.h/.cpp` | GAS miracle: Healing |
| `GA_Blessing.h/.cpp` | GAS miracle: Blessing of Protection |
| `GA_Radiance.h/.cpp` | GAS miracle: Radiance AoE |
| `NazareneNPC.h/.cpp` | NPC dialogue actor |
| `NazareneLoreItem.h/.cpp` | Interactable lore objects |
| `UNazareneDamageNumberWidget` | Floating damage numbers |
| `UNazareneEnemyHealthBarWidget` | World-space enemy health bars |
| `UNazareneSkillTreeWidget` | Skill tree UI |

## Key Source Files to Modify

| File | Changes Needed |
|------|---------------|
| `TheNazareneAAA.Build.cs` | Add `GameplayAbilities` module |
| `NazareneTypes.h` | Add XP, skill points, unlocked skills to save payload |
| `NazareneCampaignGameMode.cpp` | Expand from 4 to 7 regions; migrate to DataAsset; level streaming |
| `NazareneHUDWidget.h/.cpp` | Add skill tree, death screen, loading screen, damage numbers, enemy health bars |
| `NazarenePlayerSettings.h` | Add accessibility settings |

---

## Verification Plan

1. **Per-week gate**: Each week ends with a PIE run confirming no regressions from that week's changes
2. **Week 7 is the formal QA gate**: All 7 regions playable start-to-finish, all save slots work, 60fps sustained, no crashes in 60 minutes
3. **Week 8 is Shipping validation**: Full playthrough in packaged build (not editor), clean install test, screenshot/trailer capture confirm visual quality
4. **Release criteria** (from `docs/PACKAGING_PROFILE_PIPELINE.md`):
   - No crash repro in 60-minute soak
   - All 7 regions under frame-time budget
   - Save/load regression pass clean
   - Input validated for gamepad + KBM

