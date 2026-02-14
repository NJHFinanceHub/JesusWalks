Status: Active

# AAA Architecture Framework Review (Birth-to-Cross Campaign)

_Date: 2026-02-13_

## Executive verdict
The current Unreal codebase is a **solid vertical-slice foundation**, but it is **not yet an architecture suitable for a full AAA campaign spanning Jesus's birth through death**.

Today the project has strong baseline pillars (combat loop, save/load, HUD/menu, region progression, AI scaffolding), but major AAA-enabling architectural layers are still missing: data-driven chapter orchestration, quest/state graphing at scale, streaming/world partition strategy, narrative/cinematic pipeline, content modularization, and automated runtime validation.

## What is already structurally strong

1. **Playable core loop exists in Unreal C++**
   - Campaign flow and region progression through `ANazareneCampaignGameMode`.
   - Player combat + miracle systems and interaction hooks in `ANazarenePlayerCharacter`.
   - Save payload + subsystem model via `UNazareneSaveSubsystem` and `UNazareneSaveGame`.

2. **A migration target has been established clearly**
   - Root README and migration docs consistently identify Unreal as the authoritative runtime and legacy prototype as legacy parity reference.

3. **Tooling direction has started**
   - There are reproducibility helpers (`Tools/create_campaign_level.py`, `Tools/create_opening_chapter_layout.py`) and an asset validation utility (`Tools/validate_asset_pipeline.py`).

## Architectural blockers for full AAA scope

### 1) Narrative scale mismatch (critical)
The narrative spec spans multi-act biblical history with a Jesus-centered arc from birth to crucifixion/resurrection context, while runtime campaign architecture is still centered on a four-region static list assembled in game mode code.

**Impact:** Hard-coded region/chapter definitions will not scale to dozens of missions, cinematics, and conditional narrative branches.

### 2) Monolithic orchestration in GameMode (critical)
`ANazareneCampaignGameMode` currently owns region construction, environment spawning, story-line queueing, enemy spawn bookkeeping, chapter objective updates, and save application.

**Impact:** A single orchestrator class becomes a change hotspot, slows team parallelism, and increases regression risk as content count grows.

### 3) Content is not yet data-driven at production depth (high)
Core structs exist (`FNazareneRegionDefinition`, `FNazareneCampaignState`, save payload snapshots), but there is no broad content database strategy (DataAssets/DataTables + deterministic IDs + patch-safe versioning) for chapter-scale mission authoring.

**Impact:** Designers, narrative, and encounter teams cannot iterate independently from C++ merge cycles.

### 4) No explicit chapter/quest graph subsystem (high)
Objective text and chapter stage transitions are present, but no dedicated quest graph/state machine service exists for prerequisites, failure states, branching outcomes, and cross-chapter persistence.

**Impact:** The requested birth-to-death campaign cannot be tracked robustly across checkpoints and long-session play.

### 5) Streaming/world partition architecture is unspecified (high)
Current flow implies region swapping with spawned placeholders and actor lifecycle reset. This is adequate for prototype progression but not for seamless AAA traversal and cinematic handoffs.

**Impact:** Performance, memory, and traversal continuity risk grows with authored environments.

### 6) Production test architecture is incomplete (high)
The repository includes validation docs, but there is no visible automated Unreal test suite (functional/system/load/save migration tests) enforcing architectural contracts in CI.

**Impact:** As mission count rises, defects in save compatibility and quest continuity will escalate.

## Recommended target framework (AAA-ready)

### A. Runtime architecture split (must-have)
- **Campaign Orchestrator Subsystem** (high-level act/chapter lifecycle only).
- **Quest Graph Subsystem** (objectives, prerequisites, branching, fail/retry policies).
- **Narrative/Cinematic Subsystem** (sequence triggering, skippable states, subtitle/VO dispatch).
- **Encounter Subsystem** (spawn groups, pacing waves, boss scripts, difficulty tuning profiles).
- **Persistence Versioning Layer** (schema migration from SaveVersion N -> N+1).

### B. Data architecture
- Move chapter/region definitions into **Primary Data Assets** (or Data Tables backed by stable row IDs).
- Define immutable IDs for chapters, quests, encounters, and cinematics.
- Keep runtime state in compact save snapshots keyed by IDs, not transient actor references.

### C. Team scaling architecture
- Split modules by domain (Campaign, Combat, AI, Narrative, UI, Save, Tools).
- Keep `GameMode` thin and delegate to subsystems/services.
- Standardize event contracts (UE delegates/gameplay messages) to decouple UI/narrative/combat.

### D. Delivery architecture for requested story range
Implement campaign in content packs:
1. **Incarnation Pack** (birth and early-life narrative beats).
2. **Ministry Pack** (teaching/miracles/open-world ministry chapters).
3. **Passion Pack** (Jerusalem escalation, betrayal, trial, crucifixion).

Each pack should ship with a chapter registry, quest graph assets, encounter bundle assets, and save-schema fixtures.

## 90-day architecture hardening plan

1. **Week 1-2:** Extract quest + campaign orchestration from `ANazareneCampaignGameMode` into subsystems and reduce GameMode to bootstrapping.
2. **Week 3-4:** Introduce chapter/quest DataAssets and migrate existing four regions off hard-coded definitions.
3. **Week 5-6:** Add save migration framework (`SaveVersion` upgrader path) with automated compatibility tests.
4. **Week 7-8:** Establish streaming policy (World Partition + level streaming conventions + encounter ownership rules).
5. **Week 9-10:** Integrate narrative sequencing service (cutscene state, skip/rewind-safe checkpoints).
6. **Week 11-12:** Gate releases on automated smoke tests: load/save continuity, chapter completion, checkpoint restore, and quest branch integrity.

## Conclusion
You are **on track for a serious Unreal foundation**, but **not yet at AAA campaign architecture readiness** for a full Jesus birth-to-death production scope. The next step is not just more content—it is restructuring into data-driven, subsystem-oriented architecture so narrative scale, team concurrency, and long-term save compatibility are sustainable.
