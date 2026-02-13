#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NazareneTypes.h"
#include "NazarenePlayerCharacter.generated.h"

class ANazareneCampaignGameMode;
class ANazareneEnemyCharacter;
class ANazarenePrayerSite;
class ANazareneTravelGate;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;
class UAnimInstance;
class UStaticMeshComponent;
class USkeletalMesh;
class USoundBase;
class UNiagaraSystem;
class UEnhancedInputComponent;
struct FInputActionValue;

UENUM()
enum class ENazarenePlayerAttackType : uint8
{
    None = 0,
    Light = 1,
    Heavy = 2
};

UCLASS()
class THENAZARENEAAA_API ANazarenePlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ANazarenePlayerCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vitals")
    float MaxHealth = 120.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vitals")
    float MaxStamina = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vitals")
    float StaminaRegen = 22.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vitals")
    float StartingFaith = 35.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float WalkSpeed = 580.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float DodgeSpeed = 1120.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|Animation")
    TSoftObjectPtr<USkeletalMesh> RetargetedSkeletalMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|Animation")
    TSoftObjectPtr<USkeletalMesh> ProductionSkeletalMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|Animation")
    TSoftClassPtr<UAnimInstance> ProductionAnimBlueprint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float LightAttackDamage = 26.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float HeavyAttackDamage = 42.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float LightAttackPoiseDamage = 28.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float HeavyAttackPoiseDamage = 54.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float LightAttackStaminaCost = 18.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float HeavyAttackStaminaCost = 32.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float DodgeStaminaCost = 26.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float ParryStaminaCost = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float LockOnRange = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float LightAttackRange = 290.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float HeavyAttackRange = 340.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miracles")
    float HealAmount = 45.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miracles")
    float HealFaithCost = 18.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miracles")
    float HealCooldown = 6.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miracles")
    float BlessingFaithCost = 22.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miracles")
    float BlessingDuration = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miracles")
    float BlessingCooldown = 14.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miracles")
    float BlessingDamageMultiplier = 0.65f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miracles")
    float BlessingStaminaMultiplier = 0.7f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miracles")
    float RadianceFaithCost = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miracles")
    float RadianceCooldown = 12.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miracles")
    float RadianceDamage = 32.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miracles")
    float RadiancePoiseDamage = 38.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miracles")
    float RadianceRadius = 600.0f;

    UFUNCTION(BlueprintCallable, Category = "Player")
    void ReceiveEnemyAttack(ANazareneEnemyCharacter* Attacker, float Damage, float PostureDamage);

    UFUNCTION(BlueprintCallable, Category = "Player")
    void AddFaith(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Player")
    void SetContextHint(const FString& InHint);

    UFUNCTION(BlueprintCallable, Category = "Player")
    const FString& GetContextHint() const;

    UFUNCTION(BlueprintCallable, Category = "Player")
    void SetCampaignGameMode(ANazareneCampaignGameMode* InCampaignGameMode);

    UFUNCTION(BlueprintCallable, Category = "Player")
    void SetUnlockedMiracles(const TArray<FName>& Miracles);

    UFUNCTION(BlueprintCallable, Category = "Player")
    bool IsMiracleUnlocked(FName MiracleId) const;

    UFUNCTION(BlueprintCallable, Category = "Player|Settings")
    void ApplyUserLookAndCameraSettings(float InMouseSensitivity, bool bInInvertLookY, float InFieldOfView);

    UFUNCTION(BlueprintCallable, Category = "Player")
    float GetHealth() const;

    UFUNCTION(BlueprintCallable, Category = "Player")
    float GetMaxHealth() const;

    UFUNCTION(BlueprintCallable, Category = "Player")
    float GetStamina() const;

    UFUNCTION(BlueprintCallable, Category = "Player")
    float GetMaxStamina() const;

    UFUNCTION(BlueprintCallable, Category = "Player")
    float GetFaith() const;

    UFUNCTION(BlueprintCallable, Category = "Player")
    bool IsBlocking() const;

    UFUNCTION(BlueprintCallable, Category = "Player")
    bool IsDodging() const;

    UFUNCTION(BlueprintCallable, Category = "Player")
    bool IsAttacking() const;

    UFUNCTION(BlueprintCallable, Category = "Player")
    bool HasLockTarget() const;

    UFUNCTION(BlueprintCallable, Category = "Player")
    bool IsDefeated() const;

    UFUNCTION(BlueprintCallable, Category = "Player")
    float GetHurtTimeRemaining() const;

    UFUNCTION(BlueprintCallable, Category = "Player")
    float GetHealCooldownRemaining() const;

    UFUNCTION(BlueprintCallable, Category = "Player")
    float GetBlessingCooldownRemaining() const;

    UFUNCTION(BlueprintCallable, Category = "Player")
    float GetRadianceCooldownRemaining() const;

    UFUNCTION(BlueprintCallable, Category = "Player")
    FString GetLockTargetName() const;

    UFUNCTION(BlueprintCallable, Category = "Player")
    FNazarenePlayerSnapshot BuildSnapshot() const;

    UFUNCTION(BlueprintCallable, Category = "Player")
    void ApplySnapshot(const FNazarenePlayerSnapshot& Snapshot);

    UFUNCTION(BlueprintCallable, Category = "Player")
    bool SaveToSlot(int32 SlotId);

    UFUNCTION(BlueprintCallable, Category = "Player")
    bool LoadFromSlot(int32 SlotId);

    UFUNCTION(BlueprintCallable, Category = "Player")
    void RestAtPrayerSite(ANazarenePrayerSite* Site);

    void SetActivePrayerSite(ANazarenePrayerSite* Site);
    void ClearActivePrayerSite(ANazarenePrayerSite* Site);

    void SetActiveTravelGate(ANazareneTravelGate* Gate);
    void ClearActiveTravelGate(ANazareneTravelGate* Gate);

private:
    void TriggerPresentation(USoundBase* Sound, UNiagaraSystem* Effect, const FVector& Location, float VolumeMultiplier = 1.0f) const;

    void InitializeEnhancedInputDefaults();
    void RegisterEnhancedInputMappingContext() const;
    void BindEnhancedInput(UEnhancedInputComponent* EnhancedInputComponent);

    void OnMoveForwardInput(const FInputActionValue& Value);
    void OnMoveRightInput(const FInputActionValue& Value);
    void OnTurnInput(const FInputActionValue& Value);
    void OnLookUpInput(const FInputActionValue& Value);

    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);

    void Interact();
    void ToggleLockOn();
    void TogglePause();
    void ToggleMouseCapture();

    void StartBlock();
    void StopBlock();

    void TryLightAttack();
    void TryHeavyAttack();
    void TryDodge();
    void TryParry();
    void TryHealingMiracle();
    void TryBlessingMiracle();
    void TryRadianceMiracle();

    void TrySaveSlot1();
    void TrySaveSlot2();
    void TrySaveSlot3();
    void TryLoadSlot1();
    void TryLoadSlot2();
    void TryLoadSlot3();

    void UpdateTimers(float DeltaSeconds);
    void RegenStamina(float DeltaSeconds);
    void UpdateMovementState(float DeltaSeconds);
    void ResolvePendingAttack();
    ANazareneEnemyCharacter* FindAttackTarget(float MaxDistance, float ArcDegrees) const;
    void ValidateLockTarget();
    void ClearLockTarget();
    bool IsBusy() const;
    bool ConsumeStamina(float Cost);
    void ApplyHealthDamage(float Amount);
    void HandleDefeat();

private:
    UPROPERTY()
    TObjectPtr<UInputMappingContext> RuntimeInputMappingContext;

    UPROPERTY(EditDefaultsOnly, Category = "Input|Assets")
    TObjectPtr<UInputMappingContext> InputMappingContextAsset;

    UPROPERTY()
    TObjectPtr<UInputAction> MoveForwardInputAction;

    UPROPERTY()
    TObjectPtr<UInputAction> MoveRightInputAction;

    UPROPERTY()
    TObjectPtr<UInputAction> TurnInputAction;

    UPROPERTY()
    TObjectPtr<UInputAction> LookUpInputAction;

    UPROPERTY()
    TObjectPtr<UInputAction> InteractInputAction;

    UPROPERTY()
    TObjectPtr<UInputAction> LockOnInputAction;

    UPROPERTY()
    TObjectPtr<UInputAction> PauseInputAction;

    UPROPERTY()
    TObjectPtr<UInputAction> ToggleMouseCaptureInputAction;

    UPROPERTY()
    TObjectPtr<UInputAction> BlockInputAction;

    UPROPERTY()
    TObjectPtr<UInputAction> LightAttackInputAction;

    UPROPERTY()
    TObjectPtr<UInputAction> HeavyAttackInputAction;

    UPROPERTY()
    TObjectPtr<UInputAction> DodgeInputAction;

    UPROPERTY()
    TObjectPtr<UInputAction> ParryInputAction;

    UPROPERTY()
    TObjectPtr<UInputAction> MiracleHealInputAction;

    UPROPERTY()
    TObjectPtr<UInputAction> MiracleBlessingInputAction;

    UPROPERTY()
    TObjectPtr<UInputAction> MiracleRadianceInputAction;

    UPROPERTY()
    TObjectPtr<UInputAction> SaveSlot1InputAction;

    UPROPERTY()
    TObjectPtr<UInputAction> SaveSlot2InputAction;

    UPROPERTY()
    TObjectPtr<UInputAction> SaveSlot3InputAction;

    UPROPERTY()
    TObjectPtr<UInputAction> LoadSlot1InputAction;

    UPROPERTY()
    TObjectPtr<UInputAction> LoadSlot2InputAction;

    UPROPERTY()
    TObjectPtr<UInputAction> LoadSlot3InputAction;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UCameraComponent> FollowCamera;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> BodyMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|Audio", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USoundBase> LightAttackSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|Audio", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USoundBase> HeavyAttackSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|Audio", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USoundBase> DodgeSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|Audio", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USoundBase> MiracleSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|Audio", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USoundBase> HurtSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|VFX", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UNiagaraSystem> LightAttackVFX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|VFX", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UNiagaraSystem> HeavyAttackVFX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|VFX", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UNiagaraSystem> DodgeVFX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|VFX", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UNiagaraSystem> MiracleVFX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|VFX", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UNiagaraSystem> HurtVFX;

    UPROPERTY()
    TWeakObjectPtr<ANazareneEnemyCharacter> LockTarget;

    UPROPERTY()
    TWeakObjectPtr<ANazarenePrayerSite> ActivePrayerSite;

    UPROPERTY()
    TWeakObjectPtr<ANazareneTravelGate> ActiveTravelGate;

    UPROPERTY()
    TWeakObjectPtr<ANazareneCampaignGameMode> CampaignGameMode;

    UPROPERTY()
    float CurrentHealth = 0.0f;

    UPROPERTY()
    float CurrentStamina = 0.0f;

    UPROPERTY()
    float CurrentFaith = 0.0f;

    UPROPERTY()
    FName LastRestSiteId = NAME_None;

    TSet<FName> UnlockedMiracles;
    FString ContextHint;

    ENazarenePlayerAttackType PendingAttack = ENazarenePlayerAttackType::None;
    float AttackWindupTimer = 0.0f;
    float AttackActiveTimer = 0.0f;
    bool bAttackResolved = false;

    float AttackCooldown = 0.0f;
    float DodgeTimer = 0.0f;
    float InvulnerabilityTimer = 0.0f;
    float ParryWindowTimer = 0.0f;
    float ParryStartupTimer = 0.0f;
    float HealCooldownTimer = 0.0f;
    float BlessingTimer = 0.0f;
    float BlessingCooldownTimer = 0.0f;
    float RadianceCooldownTimer = 0.0f;
    float PerfectBlockTimer = 0.0f;
    float HurtTimer = 0.0f;

    bool bIsBlocking = false;
    FVector DodgeDirection = FVector::ZeroVector;
    float MoveForwardAxis = 0.0f;
    float MoveRightAxis = 0.0f;
    float MouseSensitivityScale = 1.0f;
    bool bInvertLookY = false;
};
