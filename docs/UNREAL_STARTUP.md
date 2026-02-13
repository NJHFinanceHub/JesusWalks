# Unreal Engine Transition: Startup Plan

This document defines the immediate, concrete next steps to begin transitioning the current Godot prototype into an Unreal Engine production pipeline. It focuses on the *minimum upfront foundation* needed to safely scale into full AAA production, co-op abilities, host migration, and Steam launch readiness.

## 1) Project & Repository Setup (Week 0)
- **Create the Unreal Engine project** (UE 5.3+ recommended) in a new repo directory (e.g., `Unreal/`)
- **Source control strategy**
  - Recommended: Perforce for binary assets
  - Alternative: Git LFS with strict asset size rules
- **Repo structure** (initial folders)
  - `Source/`, `Content/`, `Config/`, `Plugins/`, `Docs/`
- **Coding standard**
  - Establish UE C++ style + Blueprint guidelines in `Docs/Engineering/CodeStyle.md`

## 2) Core Systems Parity Plan (Week 1-2)
Rebuild core gameplay from the Godot vertical slice before expanding scope:
- Third-person movement + camera orbit + lock-on
- Stamina combat loop (timing windows)
- Prayer Site rest/reset and save slot flow
- Enemy archetypes: shield, spear, ranged, demon, boss

Output: a UE prototype scene mirroring `scenes/GalileeRegion.tscn` behavior.

## 3) Co-op & GAS Foundation (Week 2-4)
- **Gameplay Ability System (GAS)**
  - Define Ability tags for attacks, dodge, block, parry, miracle
  - Implement **co-op ability stubs** with sync-ready effects
- **Replication first**
  - Character movement, combat abilities, enemy AI states replicated
  - Basic network session creation and join flow

## 4) Host Migration Design (Week 3-5)
- **Design doc required**: session authority and migration flow
- Implement a **migration snapshot format**:
  - Player inventories and vitals
  - Enemy states (health, phase, position)
  - World progress flags (Prayer Site state, boss phases)
- Build a **simple migration prototype** in UE:
  - Host disconnect → elect new host → restore snapshot

## 5) Steam Integration (Week 4-6)
- **Steamworks SDK** integration with Online Subsystem
- Lobby, matchmaking, and session discovery
- Achievement scaffolding (placeholder IDs + hooks)

## 6) Tooling & Pipeline (Week 1-4)
- **Build pipeline** (CI for Windows builds)
- **Asset pipeline**
  - DCC tools and export rules
  - LOD, collision, and naming conventions
- **Performance budgets**
  - Define frame budget and texture/memory limits

## 7) Immediate Deliverables Checklist
- [ ] UE project created with correct folder structure
- [ ] C++ module skeleton + minimal GameMode/PlayerController
- [ ] Basic 3rd-person controller + camera orbit
- [ ] GAS module integrated and one ability synced
- [ ] Network session creation/join working locally
- [ ] Migration design doc drafted
- [ ] Steam OSS configured (placeholder)

## 8) Ownership & Roles
Suggested early roles to define for AAA scale:
- Engine/Gameplay Engineer (systems & GAS)
- Network Engineer (replication + migration)
- Tech Artist (shader + pipeline rules)
- Producer (milestone discipline, documentation)

## 9) Next Steps in This Repo
- Create a `Docs/Unreal` directory for:
  - `MigrationPlan.md`
  - `Networking/HostMigration.md`
  - `Abilities/AbilityTags.md`
  - `Pipeline/AssetRules.md`
- Track parity with the Godot vertical slice as a checklist
