#include "NazareneVFXSubsystem.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

void UNazareneVFXSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Attempt to load Niagara systems from expected content paths.
    // If the assets do not exist yet the pointers remain null and
    // SpawnEffectAtLocation / SpawnEffectAttached will log a warning.
    HealBurstSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_Heal.NS_Heal"));
    if (HealBurstSystem == nullptr)
    {
        HealBurstSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_HealBurst.NS_HealBurst"));
    }
    BlessingAuraSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_Blessing.NS_Blessing"));
    if (BlessingAuraSystem == nullptr)
    {
        BlessingAuraSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_BlessingAura.NS_BlessingAura"));
    }
    RadiancePulseSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_Radiance.NS_Radiance"));
    if (RadiancePulseSystem == nullptr)
    {
        RadiancePulseSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_RadiancePulse.NS_RadiancePulse"));
    }
    LightSlashSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_LightAttackHit.NS_LightAttackHit"));
    if (LightSlashSystem == nullptr)
    {
        LightSlashSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_LightSlash.NS_LightSlash"));
    }
    HeavySlashSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_HeavyAttackHit.NS_HeavyAttackHit"));
    if (HeavySlashSystem == nullptr)
    {
        HeavySlashSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_HeavySlash.NS_HeavySlash"));
    }
    EnemyHitReactSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_EnemyHitReact.NS_EnemyHitReact"));
    EnemyRedeemedSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_EnemyRedeemed.NS_EnemyRedeemed"));
    BossArenaHazardSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_BossArenaHazard.NS_BossArenaHazard"));
    BossPhaseTransitionSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_BossPhaseTransition.NS_BossPhaseTransition"));
    DodgeTrailSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_DodgeTrail.NS_DodgeTrail"));
    ParryFlashSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Art/VFX/NS_ParryFlash.NS_ParryFlash"));
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
