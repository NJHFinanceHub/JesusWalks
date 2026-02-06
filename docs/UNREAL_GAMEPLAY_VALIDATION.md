# Unreal Gameplay Validation Pass

## Validation date
- February 6, 2026

## Scope
- Combat cadence and damage parity sanity check against current Unreal C++ tuning.
- Save/load UI flow verification points.
- Campaign progression gating checks.

## Method
1. Source-driven combat metric pass using current values in:
   - `ANazarenePlayerCharacter`
   - `ANazareneEnemyCharacter::ConfigureFromArchetype`
2. End-to-end scenario audit across:
   - Prayer site rest/save/load loop
   - Boss completion -> travel gate unlock -> region travel
3. UI flow audit for pause menu save/load slot summaries.

Note: runtime PIE execution is not available in this environment because `UnrealEditor-Cmd.exe`
is not installed on this host. The pass is therefore deterministic/source-driven.

## Combat metric snapshot

Assumptions:
- Player baseline health = 120
- Light attack = 26 damage, 0.5s cadence
- Heavy attack = 42 damage, 0.84s cadence

| Archetype | HP | Light hits to redeem | Light TTK (s) | Heavy hits to redeem | Heavy TTK (s) | Enemy hits to defeat player | Enemy TTK (s) |
|---|---:|---:|---:|---:|---:|---:|---:|
| Ranged | 70 | 3 | 1.50 | 2 | 1.68 | 8 | 10.96 |
| Demon | 94 | 4 | 2.00 | 3 | 2.52 | 6 | 6.96 |
| Melee Shield | 110 | 5 | 2.50 | 3 | 2.52 | 8 | 12.88 |
| Spear | 86 | 4 | 2.00 | 3 | 2.52 | 6 | 9.42 |
| Boss | 420 | 17 | 8.50 | 10 | 8.40 | 5 | 7.65 |

## Findings
1. Boss fight pressure remains high by design (player must use miracles/parry windows instead of raw trading).
2. Non-boss enemy TTK is within a 1.5s-2.5s redemption band with baseline attacks, which is appropriate for migration parity.
3. Save/load front-end now supports slot summaries and action buttons in pause UI.
4. Region completion loop remains intact via boss redemption callback and travel gate enablement.

## Remaining runtime validation steps (when Unreal editor is available)
1. PIE 20-minute run for each region to validate pacing and no soft-locks.
2. Track mean fight duration by archetype (target variance within +/-15% of source-driven projection).
3. Verify save/load slot summaries update correctly after each save and after map reload.
4. Confirm pause menu input and focus behavior with gamepad + mouse/keyboard.

