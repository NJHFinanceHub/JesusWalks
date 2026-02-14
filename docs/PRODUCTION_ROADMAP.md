Status: Active

# The Nazarene Production Roadmap

## Goal
Ship a polished single-player action RPG vertical slice, then expand to full campaign content.

## Quality bar
- Real authored environments (no primitive placeholder geometry in shipped maps)
- Character models, animation sets, VFX, audio, and UI consistency
- Stable gameplay loop: explore -> fight -> rest/save -> progress story
- Clean boot flow: splash -> main menu -> load/new game -> gameplay

## Milestones

### Milestone 1: Vertical Slice (playable chapter)
Scope:
- Main menu + start/load/options/quit flow
- One full authored map (opening chapter)
- One player character kit (movement, combat, miracles)
- 2-3 enemy archetypes + one chapter boss
- Intro story sequence + quest objective flow
- Save/load checkpoints and pause menu polish

Done when:
- Fresh boot can complete opening chapter end-to-end without console errors
- 60 FPS target on dev machine in gameplay areas

### Milestone 2: Core Content Expansion
Scope:
- Remaining campaign regions with unique set dressing
- Expanded enemy roster and boss encounters
- Narrative cutscenes/dialogue and quest chain progression
- Inventory/equipment and progression balancing

Done when:
- Full campaign playable beginning to end with placeholder VO allowed

### Milestone 3: Production Polish
Scope:
- Final art pass, lighting pass, VFX/audio pass
- UX polish (accessibility, options, onboarding)
- Performance optimization and packaging pipeline hardening

Done when:
- Release candidate build passes QA checklist and crash/perf budgets

## Workstreams
- Gameplay systems (movement/combat/AI/save)
- World building (map layout, encounters, traversal)
- Narrative (story beats, mission scripting, dialogue)
- UI/UX (menu stack, HUD, settings, onboarding)
- Content production (models, rigs, animations, audio, VFX)
- Build/release (profiling, packaging, crash triage)

## Immediate Sprint 1 (start now)
- Replace placeholder startup flow with production-ready main menu routing.
- Build Opening Chapter map layout and encounter blockout.
- Integrate one environment art kit and one character model set.
- Implement quest objective pipeline for opening story beat.
- Add intro narrative presentation (camera sequence + text/dialogue scaffold).
