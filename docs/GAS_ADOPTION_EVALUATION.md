# Gameplay Ability System (GAS) Evaluation

## Decision
- Adopt GAS in a phased migration for miracles, cooldowns, buffs, and status effects.
- Keep current C++ combat loop as the parity baseline while GAS layer is introduced region-by-region.

## Why GAS is a fit
1. Miracles map naturally to `UGameplayAbility`.
2. Cooldowns and blessing/radiance modifiers map cleanly to `GameplayEffects`.
3. Future enemy debuffs, buffs, and scripted boss statuses benefit from tag-driven effect routing.
4. Multiplayer expansion is significantly safer with GAS replication patterns than bespoke replication.

## Risks
1. Migration complexity if parity scaffolding is removed too early.
2. Initial setup cost: AbilitySystemComponent ownership, attribute sets, and effect policies.
3. UI/event wiring refactor for cooldown and status presentation.

## Phased plan
1. Phase 1: Non-breaking integration
   - Add AbilitySystemComponent to player/enemy without removing existing combat code.
   - Mirror health/stamina/faith in attribute sets.
2. Phase 2: Miracles conversion
   - Convert `heal`, `blessing`, `radiance` to gameplay abilities.
   - Route cooldowns through gameplay effects.
3. Phase 3: Status system
   - Implement stagger, parry vulnerability, and blessing multipliers as gameplay effects/tags.
4. Phase 4: Legacy cleanup
   - Remove duplicate hand-rolled cooldown/status paths once parity is confirmed.

## Go/no-go criteria
- Proceed if team expects additional miracle/status complexity or co-op in scope.
- Defer if shipping target remains strictly single-player with fixed move set and minimal status growth.

