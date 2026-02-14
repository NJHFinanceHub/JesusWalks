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

private:
    UNiagaraSystem* ResolveSystem(ENazareneVFXType Type) const;

private:
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
};
