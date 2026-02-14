#include "NazareneVFXSubsystem.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

void UNazareneVFXSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Combat VFX systems
    HealBurstSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_HealBurst.NS_HealBurst"));
    BlessingAuraSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_BlessingAura.NS_BlessingAura"));
    RadiancePulseSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_RadiancePulse.NS_RadiancePulse"));
    LightSlashSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_LightSlash.NS_LightSlash"));
    HeavySlashSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_HeavySlash.NS_HeavySlash"));
    EnemyHitReactSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_EnemyHitReact.NS_EnemyHitReact"));
    EnemyRedeemedSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_EnemyRedeemed.NS_EnemyRedeemed"));
    BossArenaHazardSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_BossArenaHazard.NS_BossArenaHazard"));
    BossPhaseTransitionSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_BossPhaseTransition.NS_BossPhaseTransition"));
    DodgeTrailSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_DodgeTrail.NS_DodgeTrail"));
    ParryFlashSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_ParryFlash.NS_ParryFlash"));

    // Ambient atmospheric VFX systems (Dark Souls-inspired region atmosphere)
    AmbientDustMotesSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/Ambient/NS_DustMotes.NS_DustMotes"));
    AmbientEmbersSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/Ambient/NS_FloatingEmbers.NS_FloatingEmbers"));
    AmbientGodRaysSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/Ambient/NS_GodRays.NS_GodRays"));
    AmbientMistWispsSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/Ambient/NS_MistWisps.NS_MistWisps"));
    AmbientFallingLeavesSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/Ambient/NS_FallingLeaves.NS_FallingLeaves"));
    AmbientSandParticlesSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/Ambient/NS_SandParticles.NS_SandParticles"));
    AmbientAshFallSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/Ambient/NS_AshFall.NS_AshFall"));
    AmbientDawnRaysSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/Ambient/NS_DawnRays.NS_DawnRays"));
    AmbientHolyGlowSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/Ambient/NS_HolyGlow.NS_HolyGlow"));
    AmbientMoonlitHazeSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/Ambient/NS_MoonlitHaze.NS_MoonlitHaze"));
    AmbientTorchSparksSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/Ambient/NS_TorchSparks.NS_TorchSparks"));
    AmbientCrowdDustSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/Ambient/NS_CrowdDust.NS_CrowdDust"));
}

UNiagaraSystem* UNazareneVFXSubsystem::ResolveSystem(ENazareneVFXType Type) const
{
    switch (Type)
    {
    case ENazareneVFXType::HealBurst:
        return HealBurstSystem;
    case ENazareneVFXType::BlessingAura:
        return BlessingAuraSystem;
    case ENazareneVFXType::RadiancePulse:
        return RadiancePulseSystem;
    case ENazareneVFXType::LightSlash:
        return LightSlashSystem;
    case ENazareneVFXType::HeavySlash:
        return HeavySlashSystem;
    case ENazareneVFXType::EnemyHitReact:
        return EnemyHitReactSystem;
    case ENazareneVFXType::EnemyRedeemed:
        return EnemyRedeemedSystem;
    case ENazareneVFXType::BossArenaHazard:
        return BossArenaHazardSystem;
    case ENazareneVFXType::BossPhaseTransition:
        return BossPhaseTransitionSystem;
    case ENazareneVFXType::DodgeTrail:
        return DodgeTrailSystem;
    case ENazareneVFXType::ParryFlash:
        return ParryFlashSystem;
    case ENazareneVFXType::AmbientDustMotes:
        return AmbientDustMotesSystem;
    case ENazareneVFXType::AmbientEmbers:
        return AmbientEmbersSystem;
    case ENazareneVFXType::AmbientGodRays:
        return AmbientGodRaysSystem;
    case ENazareneVFXType::AmbientMistWisps:
        return AmbientMistWispsSystem;
    case ENazareneVFXType::AmbientFallingLeaves:
        return AmbientFallingLeavesSystem;
    case ENazareneVFXType::AmbientSandParticles:
        return AmbientSandParticlesSystem;
    case ENazareneVFXType::AmbientAshFall:
        return AmbientAshFallSystem;
    case ENazareneVFXType::AmbientDawnRays:
        return AmbientDawnRaysSystem;
    case ENazareneVFXType::AmbientHolyGlow:
        return AmbientHolyGlowSystem;
    case ENazareneVFXType::AmbientMoonlitHaze:
        return AmbientMoonlitHazeSystem;
    case ENazareneVFXType::AmbientTorchSparks:
        return AmbientTorchSparksSystem;
    case ENazareneVFXType::AmbientCrowdDust:
        return AmbientCrowdDustSystem;
    default:
        return nullptr;
    }
}

void UNazareneVFXSubsystem::SpawnEffectAtLocation(ENazareneVFXType Type, const FVector& Location, const FRotator& Rotation)
{
    UNiagaraSystem* System = ResolveSystem(Type);
    if (System == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("NazareneVFXSubsystem: No Niagara system loaded for VFX type %d"), static_cast<int32>(Type));
        return;
    }

    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), System, Location, Rotation);
}

void UNazareneVFXSubsystem::SpawnEffectAttached(ENazareneVFXType Type, USceneComponent* AttachTo)
{
    if (AttachTo == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("NazareneVFXSubsystem: SpawnEffectAttached called with null component"));
        return;
    }

    UNiagaraSystem* System = ResolveSystem(Type);
    if (System == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("NazareneVFXSubsystem: No Niagara system loaded for VFX type %d"), static_cast<int32>(Type));
        return;
    }

    UNiagaraFunctionLibrary::SpawnSystemAttached(
        System,
        AttachTo,
        NAME_None,
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        EAttachLocation::SnapToTarget,
        true
    );
}

void UNazareneVFXSubsystem::SpawnRegionAmbientVFX(const TArray<ENazareneVFXType>& AmbientTypes, const FVector& RegionCenter, float RegionRadius)
{
    ClearAmbientVFX();

    UWorld* World = GetWorld();
    if (World == nullptr)
    {
        return;
    }

    for (const ENazareneVFXType& Type : AmbientTypes)
    {
        UNiagaraSystem* System = ResolveSystem(Type);
        if (System == nullptr)
        {
            UE_LOG(LogTemp, Warning, TEXT("NazareneVFXSubsystem: No ambient Niagara system for VFX type %d"), static_cast<int32>(Type));
            continue;
        }

        // Spawn ambient VFX at multiple points across the region for full atmospheric coverage
        const int32 SpawnCount = 5;
        for (int32 Index = 0; Index < SpawnCount; ++Index)
        {
            const float AngleDeg = (360.0f / float(SpawnCount)) * float(Index);
            const float Rad = FMath::DegreesToRadians(AngleDeg);
            const float Dist = RegionRadius * 0.45f;
            const FVector SpawnLoc = RegionCenter + FVector(
                FMath::Cos(Rad) * Dist,
                FMath::Sin(Rad) * Dist,
                150.0f + float(Index % 3) * 80.0f
            );

            UNiagaraComponent* SpawnedComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                World, System, SpawnLoc, FRotator::ZeroRotator, FVector(1.0f), true, true);

            if (SpawnedComp != nullptr)
            {
                ActiveAmbientComponents.Add(SpawnedComp);
            }
        }
    }
}

void UNazareneVFXSubsystem::ClearAmbientVFX()
{
    for (TObjectPtr<USceneComponent>& Comp : ActiveAmbientComponents)
    {
        if (Comp != nullptr)
        {
            if (UNiagaraComponent* NiagaraComp = Cast<UNiagaraComponent>(Comp))
            {
                NiagaraComp->DeactivateImmediate();
            }
            Comp->DestroyComponent();
        }
    }
    ActiveAmbientComponents.Empty();
}
