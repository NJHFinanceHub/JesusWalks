Status: Active

# Opening Chapter Vertical Slice Plan (AAA Pass)

## Objective
Deliver a production-oriented Opening Chapter for `/Game/Maps/NazareneCampaign` with authored encounter pacing, character-model replacement checkpoints, and tightly directed biblical music moments.

## Map structure and traversal landmarks

### Segment A — Shoreline Awakening (0-5 min)
- **Player onboarding lane:** soft-combat tutorial in a wide shoreline corridor.
- **Landmarks:** broken fishing skiff, prayer cairn, torch gate.
- **Traversal verbs:** sprint, dodge, lock-on intro, first prayer interaction.
- **Encounter profile:** 2 single-enemy tutorials + 1 duo pack with long windups.

### Segment B — Pilgrim Causeway (5-12 min)
- **Player pressure lane:** mid-range enemy pressure and stamina checks.
- **Landmarks:** causeway arch, abandoned market tents, scripture wall.
- **Traversal verbs:** vertical hop path, side-route pickup, choke-point push.
- **Encounter profile:** 2 mixed packs (melee + ranged zealot).

### Segment C — Courtyard of Trials (12-20 min)
- **Player mastery lane:** layered arena with flanking threats.
- **Landmarks:** collapsed bell tower, consecration brazier, sealed sanctum door.
- **Traversal verbs:** spacing discipline, miracle timing, stamina pacing.
- **Encounter profile:** 3-wave arena + elite guardian warmup.

### Segment D — Sanctum Threshold (20-30 min)
- **Boss lead-in lane:** narrative pulse before capstone encounter.
- **Landmarks:** guardian mural hall, anointing basin, sanctum threshold.
- **Encounter profile:** chapter guardian boss (phase split at ~50% HP).
- **Exit:** prayer site unlock + chapter completion state update.

## Encounter pacing targets
- **Time-to-first-hit:** < 90 seconds after player gains control.
- **Rest cadence:** prayer/checkpoint every 6-8 minutes of expected progression.
- **Intensity curve:** low -> medium -> high -> peak (boss).
- **Retry fairness:** each fail respawns within <= 20 seconds of traversal.

## Character model integration checklist

### Player
- Replace placeholder skeletal mesh with production hero model (`SK_Player_Jesus`).
- Validate:
  - bound to project-compatible skeleton,
  - locomotion + combat animation blueprint references valid,
  - capsule alignment and camera boom offsets re-tuned.

### Enemies
- Replace placeholders with region set:
  - `SK_Enemy_Zealot`,
  - `SK_Enemy_Sentinel`,
  - `SK_Enemy_Guardian`.
- Validate:
  - hit reactions mapped,
  - root motion policy consistent,
  - silhouette readability at gameplay camera distance.

## “Bomb biblical music” direction (implementation-ready)

### Core musical identity
- **Low layer:** deep frame drum + taiko-style pulse (combat urgency).
- **Mid layer:** sustained harmonic pad with modal movement.
- **High layer:** choir shimmer / vocal vowel texture for sacred tone.
- **Lead motifs:** short brass/strings swells reserved for chapter beats.

### Dynamic scoring states
- `BGM_Peace` for traversal and narrative setup.
- `BGM_Tension` for pre-combat and alert states.
- `BGM_Combat` for active skirmishes.
- `BGM_Boss` for guardian encounter.
- `BGM_Victory` for chapter completion.

### Trigger map
- Shoreline spawn -> `BGM_Peace`
- First enemy aggro -> `BGM_Tension` then `BGM_Combat`
- Courtyard wave start -> `BGM_Combat` intensity +1
- Guardian intro -> crossfade to `BGM_Boss`
- Guardian defeat -> `BGM_Victory` then decay to `BGM_Peace`

## Done criteria for this planning pass
- Authoring beats locked for blockout and encounter scripting.
- Character-model replacement gates documented and testable.
- Music states + trigger mapping defined for SoundCue/MetaSound implementation.
