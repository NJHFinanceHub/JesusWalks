# Project TODO

## Bot update command (run at the end of every task)
```sh
bash -lc 'printf "\n## Updates (%s)\n- [ ] <new todo> (@claude|@codex)\n" "$(date +%F)" >> TODO.md'
```

## Completed (implemented so far)
- Core third-person movement, camera orbit, and lock-on targeting.
- Stamina-based combat loop: light/heavy attacks, dodge with invulnerability, block, parry + riposte windows.
- Enemy AI with patrol/chase/attack behavior, poise/stagger, parry vulnerability, redemption flow.
- Prayer Site system with interaction prompt, rest recovery, and enemy reset.
- Miracle system with healing miracle (Faith resource, cooldown-based).
- Galilee region prototype environment with terrain, props, ambient lighting, encounters, and HUD.
- Enemy archetypes: Roman Shieldbearer, Roman Spearman, Roman Slinger (ranged), Unclean Spirit (demon), named boss Legion Sovereign of Gerasa with multi-phase behavior.
- Timing windows for player attacks (windup/active/recovery) and enemy attacks (telegraph/parry windows); perfect-block window.
- Lightweight animation state machines for player and enemies with placeholder mesh deformation.
- Prayer Site save slots and persistence: F1-F3 save, F5-F7 load, scoped to prayer site range.

## Remaining (assignments)
- [ ] Build a full animation state machine/graph tied to proper skeletal animation assets. (@claude)
- [ ] Add navigation mesh workflows and patrol routes for enemies. (@codex)
- [ ] Create a dedicated save/load UI menu and input flow beyond keyboard-first slots. (@claude)
- [ ] Replace placeholder geometry and mesh deformation with final art pass. (@codex)
- [ ] Run and document Godot runtime validation/balance tuning for combat timings. (@claude)
- [ ] Expand additional region content beyond Galilee vertical slice (new zones/encounters). (@codex)
