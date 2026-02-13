#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NazareneTypes.h"
#include "NazareneEnemyCharacter.generated.h"

class ANazarenePlayerCharacter;
class UBehaviorTree;
class UAnimInstance;
class UNiagaraSystem;
class USkeletalMesh;
class USoundBase;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNazareneEnemyRedeemedSignature, ANazareneEnemyCharacter*, Enemy, float, FaithReward);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNazareneEnemyPhaseChangedSignature, ANazareneEnemyCharacter*, Enemy, int32, Phase);

UCLASS()
class THENAZARENEAAA_API ANazareneEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ANazareneEnemyCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
    FString EnemyName = TEXT("Roman Patrol");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
    FName SpawnId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
    ENazareneEnemyArchetype Archetype = ENazareneEnemyArchetype::MeleeShield;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
    float FaithReward = 12.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vitals")
    float MaxHealth = 90.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vitals")
    float MaxPoise = 80.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vitals")
    float PoiseRegen = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float MoveSpeed = 380.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackDamage = 15.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float PostureDamage = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float DetectionRange = 1300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackRange = 220.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float MinimumRange = 520.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackWindup = 0.4f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackRecovery = 1.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float StrikeTimingRatio = 0.72f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float ParryWindowStartRatio = 0.35f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float ParryWindowEndRatio = 0.73f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float StaggerDuration = 1.15f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float ParryVulnerabilityDuration = 1.75f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float ProjectileSpeed = 1800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    bool bCanParryAttacks = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float ShieldBlockChance = 0.45f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|Animation")
    TSoftObjectPtr<USkeletalMesh> RetargetedSkeletalMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|Animation")
    TSoftObjectPtr<USkeletalMesh> ProductionSkeletalMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|Animation")
    TSoftClassPtr<UAnimInstance> ProductionAnimBlueprint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|Audio")
    TObjectPtr<USoundBase> AttackSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|Audio")
    TObjectPtr<USoundBase> HitReactSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|Audio")
    TObjectPtr<USoundBase> RedeemedSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|VFX")
    TObjectPtr<UNiagaraSystem> AttackVFX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|VFX")
    TObjectPtr<UNiagaraSystem> HitReactVFX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|VFX")
    TObjectPtr<UNiagaraSystem> RedeemedVFX;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime")
    float CurrentHealth = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime")
    float CurrentPoise = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime")
    ENazareneEnemyState CurrentState = ENazareneEnemyState::Idle;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime")
    int32 BossPhase = 1;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FNazareneEnemyRedeemedSignature OnRedeemed;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FNazareneEnemyPhaseChangedSignature OnPhaseChanged;

    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void ConfigureFromArchetype();

    UFUNCTION(BlueprintCallable, Category = "Enemy")
    bool CanBeParried() const;

    UFUNCTION(BlueprintCallable, Category = "Enemy")
    bool IsParried() const;

    UFUNCTION(BlueprintCallable, Category = "Enemy")
    bool IsRedeemed() const;

    UFUNCTION(BlueprintCallable, Category = "Enemy")
    ENazareneEnemyState GetState() const;

    UFUNCTION(BlueprintCallable, Category = "Enemy")
    float GetStateTimerRemaining() const;

    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void OnParried(ANazarenePlayerCharacter* ByPlayer);

    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void ReceiveCombatHit(float Damage, float PoiseDamage, ANazarenePlayerCharacter* Source);

    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void ReceiveRiposte(float Damage, ANazarenePlayerCharacter* Source);

    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void ApplyKnockback(const FVector& Direction, float Force);

    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void ResetToSpawn();

    UFUNCTION(BlueprintCallable, Category = "Enemy")
    FNazareneEnemySnapshot BuildSnapshot() const;

    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void ApplySnapshot(const FNazareneEnemySnapshot& Snapshot);

private:
    void SyncTargetFromAIController();
    void TriggerPresentation(USoundBase* Sound, UNiagaraSystem* Effect, const FVector& Location, float VolumeMultiplier = 1.0f) const;
    void UpdateBossPhase();
    void ProcessChaseBehavior(float DistanceToPlayer, float DeltaSeconds);
    void BeginMeleeAttack();
    void BeginCastAttack();
    void ProcessWindup(float DeltaSeconds, bool bCasting);
    void ResolveMeleeAttack();
    void ResolveCastAttack();
    void FaceTarget(float DeltaSeconds);
    void MoveTowardTarget(float DeltaSeconds, float Speed);
    void MoveAwayFromTarget(float DeltaSeconds);
    void StrafeAroundTarget(float DeltaSeconds);
    void SlowToStop(float DeltaSeconds);
    bool TryShieldBlock(ANazarenePlayerCharacter* Source, float PoiseDamage);
    void BecomeRedeemed(ANazarenePlayerCharacter* Source, bool bGrantReward);

    float EffectiveMoveSpeed() const;
    float EffectiveAttackDamage() const;
    float EffectivePostureDamage() const;
    float EffectiveWindup() const;
    float EffectiveRecovery() const;
    float EffectiveParryStartRatio() const;
    float EffectiveParryEndRatio() const;
    float PhaseSpeedScale() const;

private:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> BodyMesh;

    UPROPERTY()
    TWeakObjectPtr<ANazarenePlayerCharacter> TargetPlayer;

    FVector SpawnLocation = FVector::ZeroVector;
    FRotator SpawnRotation = FRotator::ZeroRotator;

    float StateTimer = 0.0f;
    float WindupDuration = 0.0f;
    float WindupElapsed = 0.0f;
    bool bAttackResolved = false;
    float ShotCooldown = 0.0f;
    float DashCooldown = 0.0f;
    int32 StrafeDirectionSign = 1;
};

