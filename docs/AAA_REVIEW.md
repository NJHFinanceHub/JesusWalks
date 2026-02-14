# AAA Review (UE5 Baseline)

This review now tracks Unreal Engine 5 implementation quality only.

## Current scope
- Runtime: `unreal/TheNazareneAAA`
- Gameplay framework: C++ + Blueprint integration
- AI: Behavior Trees + Blackboards
- UI: UMG widgets backed by C++ controllers
- Save systems: Unreal `USaveGame` + project subsystems

## Direction
All legacy prototype logic has been retired from active planning. Any gameplay behavior that previously existed in non-UE formats must be re-authored using UE5-compatible systems (C++, Blueprint, Data Assets, Behavior Trees, Animation Blueprints, Niagara, and UMG).
