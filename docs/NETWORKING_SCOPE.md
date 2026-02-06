# Networking Scope Definition

## Decision (February 6, 2026)
- Shipping scope: **single-player only**.
- Co-op is explicitly out-of-scope for current parity and production milestones.

## Rationale
1. Current migration focus is parity with Godot single-player combat/campaign loop.
2. Co-op would require broad architecture changes across combat, AI authority, save flow, and UI ownership.
3. Single-player scope allows higher confidence in performance tuning and packaging readiness.

## Technical implications of single-player scope
1. No actor replication required for gameplay actors in baseline.
2. Save/load remains local profile data with no session serialization format.
3. AI and progression logic stays authoritative in local world.
4. HUD/menu stack can remain local-player centric.

## If co-op is later approved
Minimum rework checklist:
1. Add server-authoritative combat resolution path.
2. Convert save payload design to host/session-safe progression strategy.
3. Replicate key combat attributes (health, stamina, status windows, cooldowns).
4. Replace local-only pause/menu assumptions with session-safe UX.
5. Validate AI targeting/perception under multiple player pawns.

## Cost estimate for future co-op enablement
- Engineering: high (core loop + networking architecture)
- QA: high (desync, latency, host migration scenarios)
- Production risk: high unless scheduled as a dedicated milestone

