# Jesus Walks implementation plan

## Systems modules
- **Main Menu**: scripted UI with animated background, settings/credits flow, class selection, checkpoint and slot entry points.
- **Player Skill Tree**: static skill graph (`scripts/player/skill_tree.gd`) + player unlock state, XP gating, save/load persistence.
- **Level Progression**: campaign region loader with checkpoint snapshots, boss gates, region rewards, and per-region difficulty scaling.
- **Character Classes**: Shepherd / Zealot / Prophet selected at game start and applied to base stats.
- **Boss Encounters**: region guardian boss with phase changes, completion gating, rewards, and HUD messaging.
- **UI/UX**: expanded HUD for XP, class, skill state, objective and popup notifications.

## Trade-offs
- Kept prototype systems data-driven in GDScript rather than adding editor-authored assets/scenes so features remain testable in this repository without external dependencies.
- Added lightweight hazards/NPC dialogue as script-spawned primitives to avoid introducing binary assets.
