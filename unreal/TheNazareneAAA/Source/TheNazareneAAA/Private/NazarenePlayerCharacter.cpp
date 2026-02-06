#include "NazarenePlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "Kismet/GameplayStatics.h"
#include "NazareneCampaignGameMode.h"
#include "NazareneEnemyCharacter.h"
#include "NazarenePrayerSite.h"
#include "NazareneTravelGate.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
    static void AddScaledMapping(UInputMappingContext* MappingContext, UInputAction* Action, const FKey Key, float Scale)
    {
        if (MappingContext == nullptr || Action == nullptr)
        {
            return;
        }

        FEnhancedActionKeyMapping& Mapping = MappingContext->MapKey(Action, Key);
        if (!FMath::IsNearlyEqual(Scale, 1.0f))
        {
            UInputModifierScalar* Scalar = NewObject<UInputModifierScalar>(MappingContext);
            Scalar->Scalar = FVector(Scale, Scale, Scale);
            Mapping.Modifiers.Add(Scalar);
        }
    }

    static UInputAction* MakeInputAction(UObject* Owner, const TCHAR* Name, EInputActionValueType ValueType)
    {
        UInputAction* Action = NewObject<UInputAction>(Owner, Name);
        if (Action != nullptr)
        {
            Action->ValueType = ValueType;
        }
        return Action;
    }
}

ANazarenePlayerCharacter::ANazarenePlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f);
    GetCapsuleComponent()->SetCapsuleRadius(36.0f);

    BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
    BodyMesh->SetupAttachment(GetCapsuleComponent());
    BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
    BodyMesh->SetRelativeScale3D(FVector(0.8f, 0.8f, 1.8f));
    BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CapsuleMesh(TEXT("/Engine/BasicShapes/Capsule.Capsule"));
    if (CapsuleMesh.Succeeded())
    {
        BodyMesh->SetStaticMesh(CapsuleMesh.Object);
    }

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetCapsuleComponent());
    CameraBoom->TargetArmLength = 480.0f;
    CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 150.0f);
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 14.0f;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom);
    FollowCamera->bUsePawnControlRotation = false;
    FollowCamera->FieldOfView = 68.0f;

    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

    UnlockedMiracles.Add(FName(TEXT("heal")));
    InitializeEnhancedInputDefaults();
}

void ANazarenePlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;
    CurrentStamina = MaxStamina;
    CurrentFaith = StartingFaith;

    CampaignGameMode = Cast<ANazareneCampaignGameMode>(GetWorld()->GetAuthGameMode());

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->SetInputMode(FInputModeGameOnly());
        PC->bShowMouseCursor = false;
    }

    RegisterEnhancedInputMappingContext();
}

void ANazarenePlayerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    UpdateTimers(DeltaSeconds);
    RegenStamina(DeltaSeconds);
    ValidateLockTarget();
    UpdateMovementState(DeltaSeconds);

    if (PendingAttack != ENazarenePlayerAttackType::None)
    {
        if (AttackWindupTimer > 0.0f)
        {
            AttackWindupTimer = FMath::Max(0.0f, AttackWindupTimer - DeltaSeconds);
            if (AttackWindupTimer <= 0.0f && !bAttackResolved)
            {
                ResolvePendingAttack();
            }
        }

        if (AttackWindupTimer <= 0.0f)
        {
            if (!bAttackResolved)
            {
                ResolvePendingAttack();
            }
            AttackActiveTimer = FMath::Max(0.0f, AttackActiveTimer - DeltaSeconds);
            if (AttackActiveTimer <= 0.0f)
            {
                PendingAttack = ENazarenePlayerAttackType::None;
                bAttackResolved = false;
            }
        }
    }
}

void ANazarenePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (EnhancedInputComponent == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("NazarenePlayerCharacter expected UEnhancedInputComponent but received %s"), *GetNameSafe(PlayerInputComponent));
        return;
    }
    BindEnhancedInput(EnhancedInputComponent);
}

void ANazarenePlayerCharacter::InitializeEnhancedInputDefaults()
{
    RuntimeInputMappingContext = NewObject<UInputMappingContext>(this, TEXT("NazareneRuntimeInputMapping"));

    MoveForwardInputAction = MakeInputAction(this, TEXT("NazareneMoveForward"), EInputActionValueType::Axis1D);
    MoveRightInputAction = MakeInputAction(this, TEXT("NazareneMoveRight"), EInputActionValueType::Axis1D);
    TurnInputAction = MakeInputAction(this, TEXT("NazareneTurn"), EInputActionValueType::Axis1D);
    LookUpInputAction = MakeInputAction(this, TEXT("NazareneLookUp"), EInputActionValueType::Axis1D);

    InteractInputAction = MakeInputAction(this, TEXT("NazareneInteract"), EInputActionValueType::Boolean);
    LockOnInputAction = MakeInputAction(this, TEXT("NazareneLockOn"), EInputActionValueType::Boolean);
    PauseInputAction = MakeInputAction(this, TEXT("NazarenePause"), EInputActionValueType::Boolean);
    ToggleMouseCaptureInputAction = MakeInputAction(this, TEXT("NazareneToggleMouseCapture"), EInputActionValueType::Boolean);
    BlockInputAction = MakeInputAction(this, TEXT("NazareneBlock"), EInputActionValueType::Boolean);

    LightAttackInputAction = MakeInputAction(this, TEXT("NazareneLightAttack"), EInputActionValueType::Boolean);
    HeavyAttackInputAction = MakeInputAction(this, TEXT("NazareneHeavyAttack"), EInputActionValueType::Boolean);
    DodgeInputAction = MakeInputAction(this, TEXT("NazareneDodge"), EInputActionValueType::Boolean);
    ParryInputAction = MakeInputAction(this, TEXT("NazareneParry"), EInputActionValueType::Boolean);
    MiracleHealInputAction = MakeInputAction(this, TEXT("NazareneMiracleHeal"), EInputActionValueType::Boolean);
    MiracleBlessingInputAction = MakeInputAction(this, TEXT("NazareneMiracleBlessing"), EInputActionValueType::Boolean);
    MiracleRadianceInputAction = MakeInputAction(this, TEXT("NazareneMiracleRadiance"), EInputActionValueType::Boolean);

    SaveSlot1InputAction = MakeInputAction(this, TEXT("NazareneSaveSlot1"), EInputActionValueType::Boolean);
    SaveSlot2InputAction = MakeInputAction(this, TEXT("NazareneSaveSlot2"), EInputActionValueType::Boolean);
    SaveSlot3InputAction = MakeInputAction(this, TEXT("NazareneSaveSlot3"), EInputActionValueType::Boolean);
    LoadSlot1InputAction = MakeInputAction(this, TEXT("NazareneLoadSlot1"), EInputActionValueType::Boolean);
    LoadSlot2InputAction = MakeInputAction(this, TEXT("NazareneLoadSlot2"), EInputActionValueType::Boolean);
    LoadSlot3InputAction = MakeInputAction(this, TEXT("NazareneLoadSlot3"), EInputActionValueType::Boolean);

    if (RuntimeInputMappingContext == nullptr)
    {
        return;
    }

    AddScaledMapping(RuntimeInputMappingContext, MoveForwardInputAction, EKeys::W, 1.0f);
    AddScaledMapping(RuntimeInputMappingContext, MoveForwardInputAction, EKeys::S, -1.0f);
    AddScaledMapping(RuntimeInputMappingContext, MoveForwardInputAction, EKeys::Up, 1.0f);
    AddScaledMapping(RuntimeInputMappingContext, MoveForwardInputAction, EKeys::Down, -1.0f);

    AddScaledMapping(RuntimeInputMappingContext, MoveRightInputAction, EKeys::D, 1.0f);
    AddScaledMapping(RuntimeInputMappingContext, MoveRightInputAction, EKeys::A, -1.0f);
    AddScaledMapping(RuntimeInputMappingContext, MoveRightInputAction, EKeys::Right, 1.0f);
    AddScaledMapping(RuntimeInputMappingContext, MoveRightInputAction, EKeys::Left, -1.0f);

    AddScaledMapping(RuntimeInputMappingContext, TurnInputAction, EKeys::MouseX, 1.0f);
    AddScaledMapping(RuntimeInputMappingContext, LookUpInputAction, EKeys::MouseY, -1.0f);

    RuntimeInputMappingContext->MapKey(InteractInputAction, EKeys::E);
    RuntimeInputMappingContext->MapKey(LockOnInputAction, EKeys::Q);
    RuntimeInputMappingContext->MapKey(PauseInputAction, EKeys::Escape);
    RuntimeInputMappingContext->MapKey(ToggleMouseCaptureInputAction, EKeys::Tab);
    RuntimeInputMappingContext->MapKey(BlockInputAction, EKeys::LeftShift);
    RuntimeInputMappingContext->MapKey(LightAttackInputAction, EKeys::LeftMouseButton);
    RuntimeInputMappingContext->MapKey(HeavyAttackInputAction, EKeys::RightMouseButton);
    RuntimeInputMappingContext->MapKey(DodgeInputAction, EKeys::SpaceBar);
    RuntimeInputMappingContext->MapKey(ParryInputAction, EKeys::F);
    RuntimeInputMappingContext->MapKey(MiracleHealInputAction, EKeys::R);
    RuntimeInputMappingContext->MapKey(MiracleBlessingInputAction, EKeys::One);
    RuntimeInputMappingContext->MapKey(MiracleRadianceInputAction, EKeys::Two);

    RuntimeInputMappingContext->MapKey(SaveSlot1InputAction, EKeys::F1);
    RuntimeInputMappingContext->MapKey(SaveSlot2InputAction, EKeys::F2);
    RuntimeInputMappingContext->MapKey(SaveSlot3InputAction, EKeys::F3);
    RuntimeInputMappingContext->MapKey(LoadSlot1InputAction, EKeys::F5);
    RuntimeInputMappingContext->MapKey(LoadSlot2InputAction, EKeys::F6);
    RuntimeInputMappingContext->MapKey(LoadSlot3InputAction, EKeys::F7);
}

void ANazarenePlayerCharacter::RegisterEnhancedInputMappingContext() const
{
    const APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC == nullptr || RuntimeInputMappingContext == nullptr)
    {
        return;
    }

    ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
    if (LocalPlayer == nullptr)
    {
        return;
    }

    UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    if (InputSubsystem == nullptr)
    {
        return;
    }

    InputSubsystem->ClearAllMappings();
    InputSubsystem->AddMappingContext(RuntimeInputMappingContext, 0);
}

void ANazarenePlayerCharacter::BindEnhancedInput(UEnhancedInputComponent* EnhancedInputComponent)
{
    if (EnhancedInputComponent == nullptr)
    {
        return;
    }

    EnhancedInputComponent->BindAction(MoveForwardInputAction, ETriggerEvent::Triggered, this, &ANazarenePlayerCharacter::OnMoveForwardInput);
    EnhancedInputComponent->BindAction(MoveForwardInputAction, ETriggerEvent::Completed, this, &ANazarenePlayerCharacter::OnMoveForwardInput);

    EnhancedInputComponent->BindAction(MoveRightInputAction, ETriggerEvent::Triggered, this, &ANazarenePlayerCharacter::OnMoveRightInput);
    EnhancedInputComponent->BindAction(MoveRightInputAction, ETriggerEvent::Completed, this, &ANazarenePlayerCharacter::OnMoveRightInput);

    EnhancedInputComponent->BindAction(TurnInputAction, ETriggerEvent::Triggered, this, &ANazarenePlayerCharacter::OnTurnInput);
    EnhancedInputComponent->BindAction(TurnInputAction, ETriggerEvent::Completed, this, &ANazarenePlayerCharacter::OnTurnInput);

    EnhancedInputComponent->BindAction(LookUpInputAction, ETriggerEvent::Triggered, this, &ANazarenePlayerCharacter::OnLookUpInput);
    EnhancedInputComponent->BindAction(LookUpInputAction, ETriggerEvent::Completed, this, &ANazarenePlayerCharacter::OnLookUpInput);

    EnhancedInputComponent->BindAction(InteractInputAction, ETriggerEvent::Started, this, &ANazarenePlayerCharacter::Interact);
    EnhancedInputComponent->BindAction(LockOnInputAction, ETriggerEvent::Started, this, &ANazarenePlayerCharacter::ToggleLockOn);
    EnhancedInputComponent->BindAction(PauseInputAction, ETriggerEvent::Started, this, &ANazarenePlayerCharacter::TogglePause);
    EnhancedInputComponent->BindAction(ToggleMouseCaptureInputAction, ETriggerEvent::Started, this, &ANazarenePlayerCharacter::ToggleMouseCapture);

    EnhancedInputComponent->BindAction(BlockInputAction, ETriggerEvent::Started, this, &ANazarenePlayerCharacter::StartBlock);
    EnhancedInputComponent->BindAction(BlockInputAction, ETriggerEvent::Completed, this, &ANazarenePlayerCharacter::StopBlock);

    EnhancedInputComponent->BindAction(LightAttackInputAction, ETriggerEvent::Started, this, &ANazarenePlayerCharacter::TryLightAttack);
    EnhancedInputComponent->BindAction(HeavyAttackInputAction, ETriggerEvent::Started, this, &ANazarenePlayerCharacter::TryHeavyAttack);
    EnhancedInputComponent->BindAction(DodgeInputAction, ETriggerEvent::Started, this, &ANazarenePlayerCharacter::TryDodge);
    EnhancedInputComponent->BindAction(ParryInputAction, ETriggerEvent::Started, this, &ANazarenePlayerCharacter::TryParry);
    EnhancedInputComponent->BindAction(MiracleHealInputAction, ETriggerEvent::Started, this, &ANazarenePlayerCharacter::TryHealingMiracle);
    EnhancedInputComponent->BindAction(MiracleBlessingInputAction, ETriggerEvent::Started, this, &ANazarenePlayerCharacter::TryBlessingMiracle);
    EnhancedInputComponent->BindAction(MiracleRadianceInputAction, ETriggerEvent::Started, this, &ANazarenePlayerCharacter::TryRadianceMiracle);

    EnhancedInputComponent->BindAction(SaveSlot1InputAction, ETriggerEvent::Started, this, &ANazarenePlayerCharacter::TrySaveSlot1);
    EnhancedInputComponent->BindAction(SaveSlot2InputAction, ETriggerEvent::Started, this, &ANazarenePlayerCharacter::TrySaveSlot2);
    EnhancedInputComponent->BindAction(SaveSlot3InputAction, ETriggerEvent::Started, this, &ANazarenePlayerCharacter::TrySaveSlot3);
    EnhancedInputComponent->BindAction(LoadSlot1InputAction, ETriggerEvent::Started, this, &ANazarenePlayerCharacter::TryLoadSlot1);
    EnhancedInputComponent->BindAction(LoadSlot2InputAction, ETriggerEvent::Started, this, &ANazarenePlayerCharacter::TryLoadSlot2);
    EnhancedInputComponent->BindAction(LoadSlot3InputAction, ETriggerEvent::Started, this, &ANazarenePlayerCharacter::TryLoadSlot3);
}

void ANazarenePlayerCharacter::OnMoveForwardInput(const FInputActionValue& Value)
{
    MoveForward(Value.Get<float>());
}

void ANazarenePlayerCharacter::OnMoveRightInput(const FInputActionValue& Value)
{
    MoveRight(Value.Get<float>());
}

void ANazarenePlayerCharacter::OnTurnInput(const FInputActionValue& Value)
{
    Turn(Value.Get<float>());
}

void ANazarenePlayerCharacter::OnLookUpInput(const FInputActionValue& Value)
{
    LookUp(Value.Get<float>());
}

void ANazarenePlayerCharacter::ReceiveEnemyAttack(ANazareneEnemyCharacter* Attacker, float Damage, float PostureDamage)
{
    if (InvulnerabilityTimer > 0.0f)
    {
        return;
    }

    if (ParryWindowTimer > 0.0f && Attacker != nullptr && Attacker->CanBeParried())
    {
        Attacker->OnParried(this);
        AddFaith(2.0f);
        ParryWindowTimer = 0.0f;
        return;
    }

    float DamageMultiplier = 1.0f;
    float PostureMultiplier = 1.0f;
    if (BlessingTimer > 0.0f)
    {
        DamageMultiplier = BlessingDamageMultiplier;
        PostureMultiplier = BlessingStaminaMultiplier;
    }

    const float AdjustedDamage = Damage * DamageMultiplier;
    const float AdjustedPosture = PostureDamage * PostureMultiplier;

    if (bIsBlocking && CurrentStamina > 0.0f)
    {
        float StaminaLoss = AdjustedPosture * 1.2f;
        if (PerfectBlockTimer > 0.0f)
        {
            StaminaLoss *= 0.45f;
            AddFaith(1.0f);
        }
        CurrentStamina = FMath::Max(0.0f, CurrentStamina - StaminaLoss);
        if (CurrentStamina <= KINDA_SMALL_NUMBER && PerfectBlockTimer <= 0.0f)
        {
            ApplyHealthDamage(AdjustedDamage * 0.35f);
        }
        return;
    }

    ApplyHealthDamage(AdjustedDamage);
}

void ANazarenePlayerCharacter::AddFaith(float Amount)
{
    CurrentFaith = FMath::Max(0.0f, CurrentFaith + Amount);
}

void ANazarenePlayerCharacter::SetContextHint(const FString& InHint)
{
    ContextHint = InHint;
}

const FString& ANazarenePlayerCharacter::GetContextHint() const
{
    return ContextHint;
}

void ANazarenePlayerCharacter::SetCampaignGameMode(ANazareneCampaignGameMode* InCampaignGameMode)
{
    CampaignGameMode = InCampaignGameMode;
}

void ANazarenePlayerCharacter::SetUnlockedMiracles(const TArray<FName>& Miracles)
{
    UnlockedMiracles.Empty();
    UnlockedMiracles.Add(FName(TEXT("heal")));
    for (const FName Miracle : Miracles)
    {
        if (!Miracle.IsNone())
        {
            UnlockedMiracles.Add(Miracle);
        }
    }
}

bool ANazarenePlayerCharacter::IsMiracleUnlocked(FName MiracleId) const
{
    return UnlockedMiracles.Contains(MiracleId);
}

float ANazarenePlayerCharacter::GetHealth() const
{
    return CurrentHealth;
}

float ANazarenePlayerCharacter::GetMaxHealth() const
{
    return MaxHealth;
}

float ANazarenePlayerCharacter::GetStamina() const
{
    return CurrentStamina;
}

float ANazarenePlayerCharacter::GetMaxStamina() const
{
    return MaxStamina;
}

float ANazarenePlayerCharacter::GetFaith() const
{
    return CurrentFaith;
}

float ANazarenePlayerCharacter::GetHealCooldownRemaining() const
{
    return HealCooldownTimer;
}

float ANazarenePlayerCharacter::GetBlessingCooldownRemaining() const
{
    return BlessingCooldownTimer;
}

float ANazarenePlayerCharacter::GetRadianceCooldownRemaining() const
{
    return RadianceCooldownTimer;
}

FString ANazarenePlayerCharacter::GetLockTargetName() const
{
    return LockTarget.IsValid() ? LockTarget->EnemyName : FString();
}

FNazarenePlayerSnapshot ANazarenePlayerCharacter::BuildSnapshot() const
{
    FNazarenePlayerSnapshot Snapshot;
    Snapshot.Position = GetActorLocation();
    Snapshot.Health = CurrentHealth;
    Snapshot.Stamina = CurrentStamina;
    Snapshot.Faith = CurrentFaith;
    Snapshot.LastRestSiteId = LastRestSiteId;
    return Snapshot;
}

void ANazarenePlayerCharacter::ApplySnapshot(const FNazarenePlayerSnapshot& Snapshot)
{
    SetActorLocation(Snapshot.Position);
    CurrentHealth = FMath::Clamp(Snapshot.Health, 0.0f, MaxHealth);
    CurrentStamina = FMath::Clamp(Snapshot.Stamina, 0.0f, MaxStamina);
    CurrentFaith = FMath::Max(0.0f, Snapshot.Faith);
    LastRestSiteId = Snapshot.LastRestSiteId;

    AttackCooldown = 0.0f;
    DodgeTimer = 0.0f;
    InvulnerabilityTimer = 0.0f;
    ParryWindowTimer = 0.0f;
    ParryStartupTimer = 0.0f;
    HealCooldownTimer = 0.0f;
    BlessingTimer = 0.0f;
    BlessingCooldownTimer = 0.0f;
    RadianceCooldownTimer = 0.0f;
    PerfectBlockTimer = 0.0f;
    HurtTimer = 0.0f;
    PendingAttack = ENazarenePlayerAttackType::None;
    AttackWindupTimer = 0.0f;
    AttackActiveTimer = 0.0f;
    bAttackResolved = false;
    ClearLockTarget();
}

bool ANazarenePlayerCharacter::SaveToSlot(int32 SlotId)
{
    if (!ActivePrayerSite.IsValid())
    {
        SetContextHint(TEXT("You can only save while standing at a Prayer Site."));
        return false;
    }

    if (!CampaignGameMode.IsValid())
    {
        CampaignGameMode = Cast<ANazareneCampaignGameMode>(GetWorld()->GetAuthGameMode());
    }

    if (!CampaignGameMode.IsValid())
    {
        SetContextHint(TEXT("Campaign manager unavailable."));
        return false;
    }

    const bool bOk = CampaignGameMode->SaveToSlot(SlotId);
    SetContextHint(bOk ? FString::Printf(TEXT("Saved to slot %d."), SlotId) : FString::Printf(TEXT("Save failed for slot %d."), SlotId));
    return bOk;
}

bool ANazarenePlayerCharacter::LoadFromSlot(int32 SlotId)
{
    if (!ActivePrayerSite.IsValid())
    {
        SetContextHint(TEXT("You can only load while standing at a Prayer Site."));
        return false;
    }

    if (!CampaignGameMode.IsValid())
    {
        CampaignGameMode = Cast<ANazareneCampaignGameMode>(GetWorld()->GetAuthGameMode());
    }

    if (!CampaignGameMode.IsValid())
    {
        SetContextHint(TEXT("Campaign manager unavailable."));
        return false;
    }

    const bool bOk = CampaignGameMode->LoadFromSlot(SlotId);
    SetContextHint(bOk ? FString::Printf(TEXT("Loaded slot %d."), SlotId) : FString::Printf(TEXT("Load failed for slot %d."), SlotId));
    return bOk;
}

void ANazarenePlayerCharacter::RestAtPrayerSite(ANazarenePrayerSite* Site)
{
    if (Site == nullptr)
    {
        return;
    }

    SetActorLocation(Site->GetRespawnLocation());
    GetCharacterMovement()->StopMovementImmediately();

    CurrentHealth = MaxHealth;
    CurrentStamina = MaxStamina;
    HealCooldownTimer = 0.0f;
    BlessingTimer = 0.0f;
    BlessingCooldownTimer = 0.0f;
    RadianceCooldownTimer = 0.0f;
    AttackCooldown = 0.0f;
    DodgeTimer = 0.0f;
    HurtTimer = 0.0f;
    PendingAttack = ENazarenePlayerAttackType::None;
    AttackWindupTimer = 0.0f;
    AttackActiveTimer = 0.0f;
    bAttackResolved = false;
    LastRestSiteId = Site->SiteId;
    SetContextHint(Site->GetPromptMessage());

    for (TActorIterator<ANazareneEnemyCharacter> It(GetWorld()); It; ++It)
    {
        It->ResetToSpawn();
    }
    ClearLockTarget();
}

void ANazarenePlayerCharacter::SetActivePrayerSite(ANazarenePrayerSite* Site)
{
    ActivePrayerSite = Site;
    if (Site != nullptr)
    {
        SetContextHint(Site->GetPromptMessage());
    }
}

void ANazarenePlayerCharacter::ClearActivePrayerSite(ANazarenePrayerSite* Site)
{
    if (ActivePrayerSite.Get() != Site)
    {
        return;
    }
    ActivePrayerSite = nullptr;
    SetContextHint(TEXT(""));
}

void ANazarenePlayerCharacter::SetActiveTravelGate(ANazareneTravelGate* Gate)
{
    ActiveTravelGate = Gate;
    if (Gate != nullptr)
    {
        SetContextHint(Gate->PromptText);
    }
}

void ANazarenePlayerCharacter::ClearActiveTravelGate(ANazareneTravelGate* Gate)
{
    if (ActiveTravelGate.Get() != Gate)
    {
        return;
    }
    ActiveTravelGate = nullptr;
    SetContextHint(TEXT(""));
}

void ANazarenePlayerCharacter::MoveForward(float Value)
{
    MoveForwardAxis = Value;
}

void ANazarenePlayerCharacter::MoveRight(float Value)
{
    MoveRightAxis = Value;
}

void ANazarenePlayerCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}

void ANazarenePlayerCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

void ANazarenePlayerCharacter::Interact()
{
    if (ActivePrayerSite.IsValid())
    {
        RestAtPrayerSite(ActivePrayerSite.Get());
        return;
    }

    if (ActiveTravelGate.IsValid())
    {
        ActiveTravelGate->RequestTravel();
    }
}

void ANazarenePlayerCharacter::ToggleLockOn()
{
    if (LockTarget.IsValid())
    {
        ClearLockTarget();
        return;
    }

    ANazareneEnemyCharacter* Nearest = nullptr;
    float NearestDistance = LockOnRange;
    for (TActorIterator<ANazareneEnemyCharacter> It(GetWorld()); It; ++It)
    {
        ANazareneEnemyCharacter* Enemy = *It;
        if (Enemy == nullptr || Enemy->IsRedeemed())
        {
            continue;
        }
        const float Dist = FVector::Dist2D(GetActorLocation(), Enemy->GetActorLocation());
        if (Dist < NearestDistance)
        {
            NearestDistance = Dist;
            Nearest = Enemy;
        }
    }

    LockTarget = Nearest;
}

void ANazarenePlayerCharacter::TogglePause()
{
    const bool bPaused = UGameplayStatics::IsGamePaused(this);
    UGameplayStatics::SetGamePaused(this, !bPaused);

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (!bPaused)
        {
            PC->bShowMouseCursor = true;
            PC->SetInputMode(FInputModeGameAndUI());
        }
        else
        {
            PC->bShowMouseCursor = false;
            PC->SetInputMode(FInputModeGameOnly());
        }
    }
}

void ANazarenePlayerCharacter::ToggleMouseCapture()
{
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        const bool bShow = !PC->bShowMouseCursor;
        PC->bShowMouseCursor = bShow;
        if (bShow)
        {
            PC->SetInputMode(FInputModeGameAndUI());
        }
        else
        {
            PC->SetInputMode(FInputModeGameOnly());
        }
    }
}

void ANazarenePlayerCharacter::StartBlock()
{
    bIsBlocking = true;
    PerfectBlockTimer = 0.18f;
}

void ANazarenePlayerCharacter::StopBlock()
{
    bIsBlocking = false;
}

void ANazarenePlayerCharacter::TryLightAttack()
{
    if (IsBusy() || PendingAttack != ENazarenePlayerAttackType::None)
    {
        return;
    }
    if (!ConsumeStamina(LightAttackStaminaCost))
    {
        return;
    }
    PendingAttack = ENazarenePlayerAttackType::Light;
    AttackWindupTimer = 0.15f;
    AttackActiveTimer = 0.2f;
    bAttackResolved = false;
    AttackCooldown = 0.5f;
}

void ANazarenePlayerCharacter::TryHeavyAttack()
{
    if (IsBusy() || PendingAttack != ENazarenePlayerAttackType::None)
    {
        return;
    }
    if (!ConsumeStamina(HeavyAttackStaminaCost))
    {
        return;
    }
    PendingAttack = ENazarenePlayerAttackType::Heavy;
    AttackWindupTimer = 0.3f;
    AttackActiveTimer = 0.23f;
    bAttackResolved = false;
    AttackCooldown = 0.84f;
}

void ANazarenePlayerCharacter::TryDodge()
{
    if (IsBusy())
    {
        return;
    }
    if (!ConsumeStamina(DodgeStaminaCost))
    {
        return;
    }

    FVector DodgeVector = GetActorForwardVector();
    if (AController* C = GetController())
    {
        const FRotator YawRotation(0.0f, C->GetControlRotation().Yaw, 0.0f);
        const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        const FVector InputDir = Forward * MoveForwardAxis + Right * MoveRightAxis;
        if (!InputDir.IsNearlyZero())
        {
            DodgeVector = InputDir.GetSafeNormal();
        }
    }

    DodgeDirection = DodgeVector;
    LaunchCharacter(DodgeDirection * DodgeSpeed, true, false);
    PendingAttack = ENazarenePlayerAttackType::None;
    AttackCooldown = 0.28f;
    DodgeTimer = 0.28f;
    InvulnerabilityTimer = 0.22f;
}

void ANazarenePlayerCharacter::TryParry()
{
    if (IsBusy())
    {
        return;
    }
    if (!ConsumeStamina(ParryStaminaCost))
    {
        return;
    }
    AttackCooldown = 0.42f;
    ParryStartupTimer = 0.08f;
    ParryWindowTimer = 0.0f;
}

void ANazarenePlayerCharacter::TryHealingMiracle()
{
    if (HealCooldownTimer > 0.0f || CurrentFaith < HealFaithCost || CurrentHealth >= MaxHealth)
    {
        return;
    }

    CurrentFaith -= HealFaithCost;
    CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + HealAmount);
    HealCooldownTimer = HealCooldown;
    AttackCooldown = FMath::Max(AttackCooldown, 0.35f);
}

void ANazarenePlayerCharacter::TryBlessingMiracle()
{
    if (!IsMiracleUnlocked(FName(TEXT("blessing"))))
    {
        SetContextHint(TEXT("Blessing miracle not yet unlocked."));
        return;
    }
    if (BlessingCooldownTimer > 0.0f || CurrentFaith < BlessingFaithCost)
    {
        return;
    }
    CurrentFaith -= BlessingFaithCost;
    BlessingTimer = BlessingDuration;
    BlessingCooldownTimer = BlessingCooldown;
    AttackCooldown = FMath::Max(AttackCooldown, 0.35f);
}

void ANazarenePlayerCharacter::TryRadianceMiracle()
{
    if (!IsMiracleUnlocked(FName(TEXT("radiance"))))
    {
        SetContextHint(TEXT("Radiance miracle not yet unlocked."));
        return;
    }
    if (RadianceCooldownTimer > 0.0f || CurrentFaith < RadianceFaithCost)
    {
        return;
    }

    CurrentFaith -= RadianceFaithCost;
    RadianceCooldownTimer = RadianceCooldown;
    AttackCooldown = FMath::Max(AttackCooldown, 0.45f);

    for (TActorIterator<ANazareneEnemyCharacter> It(GetWorld()); It; ++It)
    {
        ANazareneEnemyCharacter* Enemy = *It;
        if (Enemy == nullptr || Enemy->IsRedeemed())
        {
            continue;
        }
        const FVector ToEnemy = Enemy->GetActorLocation() - GetActorLocation();
        if (ToEnemy.Size() <= RadianceRadius)
        {
            Enemy->ReceiveHit(RadianceDamage, RadiancePoiseDamage, this);
            Enemy->ApplyKnockback(ToEnemy.GetSafeNormal2D(), 450.0f);
        }
    }
}

void ANazarenePlayerCharacter::TrySaveSlot1()
{
    SaveToSlot(1);
}

void ANazarenePlayerCharacter::TrySaveSlot2()
{
    SaveToSlot(2);
}

void ANazarenePlayerCharacter::TrySaveSlot3()
{
    SaveToSlot(3);
}

void ANazarenePlayerCharacter::TryLoadSlot1()
{
    LoadFromSlot(1);
}

void ANazarenePlayerCharacter::TryLoadSlot2()
{
    LoadFromSlot(2);
}

void ANazarenePlayerCharacter::TryLoadSlot3()
{
    LoadFromSlot(3);
}

void ANazarenePlayerCharacter::UpdateTimers(float DeltaSeconds)
{
    AttackCooldown = FMath::Max(0.0f, AttackCooldown - DeltaSeconds);
    DodgeTimer = FMath::Max(0.0f, DodgeTimer - DeltaSeconds);
    InvulnerabilityTimer = FMath::Max(0.0f, InvulnerabilityTimer - DeltaSeconds);
    HealCooldownTimer = FMath::Max(0.0f, HealCooldownTimer - DeltaSeconds);
    BlessingTimer = FMath::Max(0.0f, BlessingTimer - DeltaSeconds);
    BlessingCooldownTimer = FMath::Max(0.0f, BlessingCooldownTimer - DeltaSeconds);
    RadianceCooldownTimer = FMath::Max(0.0f, RadianceCooldownTimer - DeltaSeconds);
    PerfectBlockTimer = FMath::Max(0.0f, PerfectBlockTimer - DeltaSeconds);
    HurtTimer = FMath::Max(0.0f, HurtTimer - DeltaSeconds);

    if (ParryStartupTimer > 0.0f)
    {
        ParryStartupTimer = FMath::Max(0.0f, ParryStartupTimer - DeltaSeconds);
        if (ParryStartupTimer <= 0.0f)
        {
            ParryWindowTimer = 0.23f;
        }
    }
    else
    {
        ParryWindowTimer = FMath::Max(0.0f, ParryWindowTimer - DeltaSeconds);
    }
}

void ANazarenePlayerCharacter::RegenStamina(float DeltaSeconds)
{
    if (DodgeTimer > 0.0f || bIsBlocking || AttackCooldown > 0.0f)
    {
        return;
    }

    float RegenRate = StaminaRegen;
    if (BlessingTimer > 0.0f)
    {
        RegenRate *= 1.35f;
    }
    CurrentStamina = FMath::Min(MaxStamina, CurrentStamina + RegenRate * DeltaSeconds);
}

void ANazarenePlayerCharacter::ResolvePendingAttack()
{
    ANazareneEnemyCharacter* Enemy = nullptr;
    if (PendingAttack == ENazarenePlayerAttackType::Light)
    {
        Enemy = FindAttackTarget(LightAttackRange, 70.0f);
    }
    else if (PendingAttack == ENazarenePlayerAttackType::Heavy)
    {
        Enemy = FindAttackTarget(HeavyAttackRange, 80.0f);
    }
    else
    {
        return;
    }

    bAttackResolved = true;
    if (Enemy == nullptr)
    {
        return;
    }

    if (PendingAttack == ENazarenePlayerAttackType::Light)
    {
        if (Enemy->IsParried())
        {
            Enemy->ReceiveRiposte(HeavyAttackDamage * 1.08f, this);
        }
        else
        {
            Enemy->ReceiveHit(LightAttackDamage, LightAttackPoiseDamage, this);
            AddFaith(1.0f);
        }
    }
    else
    {
        if (Enemy->IsParried())
        {
            Enemy->ReceiveRiposte(HeavyAttackDamage * 1.5f, this);
        }
        else
        {
            Enemy->ReceiveHit(HeavyAttackDamage, HeavyAttackPoiseDamage, this);
            AddFaith(1.5f);
        }
    }
}

ANazareneEnemyCharacter* ANazarenePlayerCharacter::FindAttackTarget(float MaxDistance, float ArcDegrees) const
{
    if (LockTarget.IsValid() && !LockTarget->IsRedeemed())
    {
        const float Dist = FVector::Dist2D(GetActorLocation(), LockTarget->GetActorLocation());
        if (Dist <= MaxDistance + 70.0f)
        {
            return LockTarget.Get();
        }
    }

    ANazareneEnemyCharacter* BestTarget = nullptr;
    float BestDistance = FLT_MAX;
    const FVector Forward = GetActorForwardVector().GetSafeNormal2D();

    for (TActorIterator<ANazareneEnemyCharacter> It(GetWorld()); It; ++It)
    {
        ANazareneEnemyCharacter* Enemy = *It;
        if (Enemy == nullptr || Enemy->IsRedeemed())
        {
            continue;
        }

        FVector ToEnemy = Enemy->GetActorLocation() - GetActorLocation();
        ToEnemy.Z = 0.0f;
        const float Dist = ToEnemy.Size();
        if (Dist <= 0.1f || Dist > MaxDistance)
        {
            continue;
        }

        const float Dot = FVector::DotProduct(Forward, ToEnemy.GetSafeNormal());
        const float Angle = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(Dot, -1.0f, 1.0f)));
        if (Angle > ArcDegrees * 0.5f)
        {
            continue;
        }

        if (Dist < BestDistance)
        {
            BestDistance = Dist;
            BestTarget = Enemy;
        }
    }
    return BestTarget;
}

void ANazarenePlayerCharacter::ValidateLockTarget()
{
    if (!LockTarget.IsValid())
    {
        return;
    }
    if (LockTarget->IsRedeemed())
    {
        ClearLockTarget();
        return;
    }

    const float Dist = FVector::Dist2D(GetActorLocation(), LockTarget->GetActorLocation());
    if (Dist > LockOnRange * 1.2f)
    {
        ClearLockTarget();
    }
}

void ANazarenePlayerCharacter::ClearLockTarget()
{
    LockTarget = nullptr;
}

bool ANazarenePlayerCharacter::IsBusy() const
{
    return AttackCooldown > 0.0f || DodgeTimer > 0.0f || HurtTimer > 0.0f;
}

bool ANazarenePlayerCharacter::ConsumeStamina(float Cost)
{
    if (CurrentStamina < Cost)
    {
        return false;
    }
    CurrentStamina -= Cost;
    return true;
}

void ANazarenePlayerCharacter::ApplyHealthDamage(float Amount)
{
    CurrentHealth = FMath::Max(0.0f, CurrentHealth - Amount);
    HurtTimer = 0.22f;
    if (CurrentHealth <= 0.01f)
    {
        HandleDefeat();
    }
}

void ANazarenePlayerCharacter::HandleDefeat()
{
    ANazarenePrayerSite* ClosestSite = nullptr;
    float ClosestDistanceSq = TNumericLimits<float>::Max();

    for (TActorIterator<ANazarenePrayerSite> It(GetWorld()); It; ++It)
    {
        ANazarenePrayerSite* Site = *It;
        const float DistSq = FVector::DistSquared(GetActorLocation(), Site->GetActorLocation());
        if (DistSq < ClosestDistanceSq)
        {
            ClosestDistanceSq = DistSq;
            ClosestSite = Site;
        }
    }

    if (ClosestSite != nullptr)
    {
        RestAtPrayerSite(ClosestSite);
    }
    else
    {
        SetActorLocation(FVector(0.0f, 0.0f, 180.0f));
        CurrentHealth = MaxHealth;
        CurrentStamina = MaxStamina;
    }
}

void ANazarenePlayerCharacter::UpdateMovementState(float DeltaSeconds)
{
    bIsBlocking = bIsBlocking && DodgeTimer <= 0.0f && PendingAttack == ENazarenePlayerAttackType::None;

    float MovementMultiplier = 1.0f;
    if (bIsBlocking)
    {
        MovementMultiplier = FMath::Min(MovementMultiplier, 0.45f);
    }
    if (PendingAttack != ENazarenePlayerAttackType::None && AttackWindupTimer > 0.0f)
    {
        MovementMultiplier = FMath::Min(MovementMultiplier, 0.28f);
    }
    else if (PendingAttack != ENazarenePlayerAttackType::None)
    {
        MovementMultiplier = FMath::Min(MovementMultiplier, 0.62f);
    }
    if (HurtTimer > 0.0f)
    {
        MovementMultiplier = FMath::Min(MovementMultiplier, 0.35f);
    }

    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed * MovementMultiplier;

    if (AController* C = GetController())
    {
        const FRotator YawRotation(0.0f, C->GetControlRotation().Yaw, 0.0f);
        const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        if (FMath::Abs(MoveForwardAxis) > KINDA_SMALL_NUMBER)
        {
            AddMovementInput(Forward, MoveForwardAxis);
        }
        if (FMath::Abs(MoveRightAxis) > KINDA_SMALL_NUMBER)
        {
            AddMovementInput(Right, MoveRightAxis);
        }
    }

    if (LockTarget.IsValid() && !LockTarget->IsRedeemed())
    {
        FVector ToTarget = LockTarget->GetActorLocation() - GetActorLocation();
        ToTarget.Z = 0.0f;
        if (!ToTarget.IsNearlyZero())
        {
            const FRotator Desired = ToTarget.Rotation();
            const FRotator NewRot = FMath::RInterpTo(GetActorRotation(), Desired, DeltaSeconds, 14.0f);
            SetActorRotation(FRotator(0.0f, NewRot.Yaw, 0.0f));
        }
    }
}
