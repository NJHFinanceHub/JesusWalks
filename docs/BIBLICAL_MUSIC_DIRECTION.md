Status: Active

# Biblical Music Direction (Production)

## Goal
Create a high-impact, sacred-epic soundtrack profile (“bomb biblical music”) that supports gameplay clarity while elevating emotional tone.

## Technical constraints
- Must loop cleanly and survive rapid state transitions.
- Must prioritize combat readability over musical density.
- Must avoid masking SFX telemetry (parry, dodge, enemy telegraph).

## Unreal implementation blueprint

### Required audio assets
- `S_BGM_Peace_Loop`
- `S_BGM_Tension_Loop`
- `S_BGM_Combat_Loop`
- `S_BGM_Boss_Loop`
- `S_BGM_Victory_Sting`

### Required cue/metasound graph assets
- `SC_BGM_Peace`
- `SC_BGM_Tension`
- `SC_BGM_Combat`
- `SC_BGM_Boss`
- `SC_BGM_Victory`
- `MS_BGM_StateRouter`

### Mix policy
- Music bus peaks target: **-12 dBFS** in gameplay.
- Sidechain ducking on `UI/VO critical` classes.
- Combat rise limited to +2.5 dB over exploration baseline.

### Runtime state machine contract
- `MusicState=Peace|Tension|Combat|Boss|Victory`
- Campaign runtime now exposes `ANazareneCampaignGameMode::GetMusicState()` for audio router polling and HUD/debug verification.
- Crossfade defaults:
  - Peace->Tension: 1.8s
  - Tension->Combat: 0.8s
  - Combat->Boss: 2.5s
  - Boss->Victory: immediate sting + 1.5s tail
  - Victory->Peace: 3.0s
- Current C++ hooks update state at region load (Peace), prayer-site consecration (Tension), player defeat (Tension), and boss redemption (Victory).

## Composition notes
- Keep harmonic language rooted in dorian/phrygian colors.
- Use choir pads sparingly in traversal; fuller voicing in boss phases.
- Layer hand percussion for momentum; avoid constant full-spectrum impact.

## Validation checklist
- [ ] No audible clicks at loop points.
- [ ] Transition latency under 1 frame of game-state event dispatch.
- [ ] Combat SFX remains intelligible during max-intensity music state.
- [ ] Boss transition perceptibly escalates emotional intensity.
