#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "NazareneTypes.h"
#include "NazareneVFXSubsystem.generated.h"

class UNiagaraSystem;

UCLASS()
class THENAZARENEAAA_API UNazareneVFXSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "VFX")
    void SpawnEffectAtLocation(ENazareneVFXType Type, const FVector& Location, const FRotator& Rotation = FRotator::ZeroRotator);

    UFUNCTION(BlueprintCallable, Category = "VFX")
    void SpawnEffectAttached(ENazareneVFXType Type, USceneComponent* AttachTo);

    /** Spawn persistent ambient VFX for a region atmosphere. */
    UFUNCTION(BlueprintCallable, Category = "VFX|Atmosphere")
    void SpawnRegionAmbientVFX(const TArray<ENazareneVFXType>& AmbientTypes, const FVector& RegionCenter, float RegionRadius = 3000.0f);

    /** Clear all active ambient VFX components. */
    UFUNCTION(BlueprintCallable, Category = "VFX|Atmosphere")
    void ClearAmbientVFX();

private:
    UNiagaraSystem* ResolveSystem(ENazareneVFXType Type) const;

private:
    // Combat VFX systems
    UPROPERTY()
    TObjectPtr<UNiagaraSystem> HealBurstSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> BlessingAuraSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> RadiancePulseSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> LightSlashSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> HeavySlashSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> EnemyHitReactSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> EnemyRedeemedSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> BossArenaHazardSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> BossPhaseTransitionSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> DodgeTrailSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> ParryFlashSystem;

    // Ambient atmospheric VFX systems
    UPROPERTY()
    TObjectPtr<UNiagaraSystem> AmbientDustMotesSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> AmbientEmbersSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> AmbientGodRaysSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> AmbientMistWispsSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> AmbientFallingLeavesSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> AmbientSandParticlesSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> AmbientAshFallSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> AmbientDawnRaysSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> AmbientHolyGlowSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> AmbientMoonlitHazeSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> AmbientTorchSparksSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> AmbientCrowdDustSystem;

    /** Active ambient VFX component handles for cleanup on region transition. */
    UPROPERTY()
    TArray<TObjectPtr<USceneComponent>> ActiveAmbientComponents;
};
