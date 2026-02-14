#include "NazarenePlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/SkeletalMesh.h"
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
#include "Materials/MaterialInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NazareneAbilitySystemComponent.h"
#include "NazareneAssetResolver.h"
#include "NazareneAttributeSet.h"
#include "NazareneCampaignGameMode.h"
#include "NazareneEnemyCharacter.h"
#include "NazareneHUD.h"
#include "NazareneNPC.h"
#include "NazarenePlayerAnimInstance.h"
#include "NazarenePrayerSite.h"
#include "NazareneSkillTree.h"
#include "NazareneSettingsSubsystem.h"
#include "NazareneTravelGate.h"
#include "GA_NazareneHeal.h"
#include "GA_NazareneBlessing.h"
#include "GA_NazareneRadiance.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
    static bool ContainsModernWeaponTerm(const FString& InPath)
    {
        const FString Lower = InPath.ToLower();
        const TCHAR* WeaponTerms[] = { TEXT("rifle"), TEXT("pistol"), TEXT("shotgun"), TEXT("sniper"), TEXT("smg"), TEXT("assault"), TEXT("carbine"), TEXT("ak"), TEXT("m4"), TEXT("gun") };
        for (const TCHAR* Term : WeaponTerms)
        {
            if (Lower.Contains(Term))
            {
                return true;
            }
        }
        return false;
    }

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

    AbilitySystemComponent = CreateDefaultSubobject<UNazareneAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AttributeSet = CreateDefaultSubobject<UNazareneAttributeSet>(TEXT("AttributeSet"));

    GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f);
    GetCapsuleComponent()->SetCapsuleRadius(36.0f);

    BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
    BodyMesh->SetupAttachment(GetCapsuleComponent());
    BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -56.0f));
    BodyMesh->SetRelativeScale3D(FVector(0.44f, 0.44f, 1.18f));
    BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
    HeadMesh->SetupAttachment(GetCapsuleComponent());
    HeadMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 24.0f));
    HeadMesh->SetRelativeScale3D(FVector(0.33f, 0.33f, 0.36f));
    HeadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    MantleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MantleMesh"));
    MantleMesh->SetupAttachment(GetCapsuleComponent());
    MantleMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -24.0f));
    MantleMesh->SetRelativeScale3D(FVector(0.7f, 0.44f, 0.18f));
    MantleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    RobeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RobeMesh"));
    RobeMesh->SetupAttachment(GetCapsuleComponent());
    RobeMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
    RobeMesh->SetRelativeScale3D(FVector(0.7f, 0.7f, 1.58f));
    RobeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    SashMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SashMesh"));
    SashMesh->SetupAttachment(GetCapsuleComponent());
    SashMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -38.0f));
    SashMesh->SetRelativeScale3D(FVector(0.46f, 0.46f, 0.08f));
    SashMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    StaffMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaffMesh"));
    StaffMesh->SetupAttachment(GetCapsuleComponent());
    StaffMesh->SetRelativeLocation(FVector(22.0f, 24.0f, -48.0f));
    StaffMesh->SetRelativeRotation(FRotator(0.0f, 10.0f, -4.0f));
    StaffMesh->SetRelativeScale3D(FVector(0.06f, 0.06f, 2.05f));
    StaffMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CapsuleMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMesh(TEXT("/Engine/BasicShapes/Cone.Cone"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CapsuleMesh.Succeeded())
    {
        BodyMesh->SetStaticMesh(CapsuleMesh.Object);
        SashMesh->SetStaticMesh(CapsuleMesh.Object);
        StaffMesh->SetStaticMesh(CapsuleMesh.Object);
    }
    if (SphereMesh.Succeeded())
    {
        HeadMesh->SetStaticMesh(SphereMesh.Object);
    }
    if (ConeMesh.Succeeded())
    {
        RobeMesh->SetStaticMesh(ConeMesh.Object);
    }
    if (CubeMesh.Succeeded())
    {
        MantleMesh->SetStaticMesh(CubeMesh.Object);
    }

    ConfigureProxyVisuals();

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetCapsuleComponent());
    CameraBoom->TargetArmLength = FreeCameraArmLength;
    CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 164.0f);
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 13.0f;
    CameraBoom->bEnableCameraRotationLag = true;
    CameraBoom->CameraRotationLagSpeed = 11.5f;
    CameraBoom->CameraLagMaxDistance = 120.0f;
    CameraBoom->bUseCameraLagSubstepping = true;
    CameraBoom->CameraLagMaxTimeStep = 1.0f / 60.0f;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom);
    FollowCamera->bUsePawnControlRotation = false;
    FollowCamera->FieldOfView = BaseCameraFOV;

    GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -92.0f));
    GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
    GetMesh()->SetAnimInstanceClass(UNazarenePlayerAnimInstance::StaticClass());

    if (!ProductionSkeletalMesh.ToSoftObjectPath().IsValid())
    {
        const FString ResolvedPlayerMesh = NazareneAssetResolver::ResolveObjectPath(
            TEXT("PlayerSkeletalMesh"),
            TEXT("/Game/Art/Characters/Player/SK_BiblicalHero.SK_BiblicalHero"),
            {
                TEXT("/Game/Characters/MedievalOrientalArmour/SK_MOH_Hero.SK_MOH_Hero"),
                TEXT("/Game/Characters/MedievalOrientalArmour/Meshes/SK_MOH_Hero.SK_MOH_Hero"),
                TEXT("/Game/MedievalOrientalArmour/SK_MOH_Hero.SK_MOH_Hero")
            });
        const FString SafePlayerMesh = ContainsModernWeaponTerm(ResolvedPlayerMesh)
            ? TEXT("/Game/Art/Characters/Player/SK_BiblicalHero.SK_BiblicalHero")
            : ResolvedPlayerMesh;
        ProductionSkeletalMesh = TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(SafePlayerMesh));
    }
    if (!ProductionAnimBlueprint.ToSoftObjectPath().IsValid())
    {
        // Keep the default C++ anim instance class unless an explicit override is configured.
        const FString ResolvedPlayerAnimBP = NazareneAssetResolver::ResolveObjectPath(
            TEXT("PlayerAnimBlueprint"),
            TEXT(""),
            {});
        if (!ResolvedPlayerAnimBP.IsEmpty())
        {
            ProductionAnimBlueprint = TSoftClassPtr<UAnimInstance>(FSoftObjectPath(ResolvedPlayerAnimBP));
        }
    }

    if (LightAttackSound == nullptr)
    {
        LightAttackSound = LoadObject<USoundBase>(nullptr, TEXT("/Game/Audio/SFX/S_AttackWhoosh.S_AttackWhoosh"));
    }
    if (HeavyAttackSound == nullptr)
    {
        HeavyAttackSound = LoadObject<USoundBase>(nullptr, TEXT("/Game/Audio/SFX/S_Impact.S_Impact"));
    }
    if (DodgeSound == nullptr)
    {
        DodgeSound = LoadObject<USoundBase>(nullptr, TEXT("/Game/Audio/SFX/S_Dodge.S_Dodge"));
    }
    if (MiracleSound == nullptr)
    {
        MiracleSound = LoadObject<USoundBase>(nullptr, TEXT("/Game/Audio/SFX/S_MiracleShimmer.S_MiracleShimmer"));
    }
    if (HurtSound == nullptr)
    {
        HurtSound = LoadObject<USoundBase>(nullptr, TEXT("/Game/Audio/SFX/S_Hurt.S_Hurt"));
    }

    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
    GetCharacterMovement()->MaxAcceleration = 2200.0f;
    GetCharacterMovement()->BrakingDecelerationWalking = 1800.0f;
    GetCharacterMovement()->GroundFriction = 6.0f;
    GetCharacterMovement()->BrakingFrictionFactor = 1.0f;
    GetCharacterMovement()->AirControl = 0.2f;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

    CampaignBaseMaxHealth = MaxHealth;
    CampaignBaseMaxStamina = MaxStamina;

    UnlockedMiracles.Add(FName(TEXT("heal")));
    InitializeEnhancedInputDefaults();
}

void ANazarenePlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    DefaultActorScale = GetActorScale3D();

    if (AbilitySystemComponent != nullptr)
    {
        AbilitySystemComponent->InitializeForActor(this, this);
        AbilitySystemComponent->GrantDefaultAbilities();
    }

    bool bAppliedCharacterMesh = false;
    if (ProductionSkeletalMesh.ToSoftObjectPath().IsValid())
    {
        if (USkeletalMesh* MeshAsset = ProductionSkeletalMesh.LoadSynchronous())
        {
            GetMesh()->SetSkeletalMesh(MeshAsset);
            bAppliedCharacterMesh = true;
        }
    }

    if (!bAppliedCharacterMesh && RetargetedSkeletalMesh.ToSoftObjectPath().IsValid())
    {
        if (USkeletalMesh* MeshAsset = RetargetedSkeletalMesh.LoadSynchronous())
        {
            GetMesh()->SetSkeletalMesh(MeshAsset);
            bAppliedCharacterMesh = true;
        }
    }

    if (ProductionAnimBlueprint.ToSoftObjectPath().IsValid())
    {
        if (UClass* AnimClass = ProductionAnimBlueprint.LoadSynchronous())
        {
            GetMesh()->SetAnimInstanceClass(AnimClass);
        }
    }

    if (bAppliedCharacterMesh)
    {
        if (UMaterialInterface* RobeMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Art/Materials/MI_Character_JesusRobe.MI_Character_JesusRobe")))
        {
            GetMesh()->SetMaterial(0, RobeMaterial);
        }
    }

    SetProxyVisualsHidden(bAppliedCharacterMesh);

    ApplySkillModifiers();
    CurrentHealth = MaxHealth;
    CurrentStamina = MaxStamina;
    CurrentFaith = StartingFaith;

    if (AttributeSet != nullptr)
    {
        AttributeSet->SetMaxHealth(MaxHealth);
        AttributeSet->SetHealth(CurrentHealth);
        AttributeSet->SetMaxStamina(MaxStamina);
        AttributeSet->SetStamina(CurrentStamina);
        AttributeSet->SetFaith(CurrentFaith);
    }

    CampaignGameMode = Cast<ANazareneCampaignGameMode>(GetWorld()->GetAuthGameMode());

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        bool bMenuVisible = false;
        if (ANazareneHUD* HUD = Cast<ANazareneHUD>(PC->GetHUD()))
        {
            bMenuVisible = HUD->IsStartMenuVisible() || HUD->IsPauseMenuVisible();
        }

        if (bMenuVisible)
        {
            FInputModeGameAndUI InputMode;
            InputMode.SetHideCursorDuringCapture(false);
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
            PC->bEnableClickEvents = true;
            PC->bEnableMouseOverEvents = true;
            PC->CurrentMouseCursor = EMouseCursor::None;
        }
        else
        {
            PC->SetInputMode(FInputModeGameOnly());
            PC->bShowMouseCursor = false;
            PC->bEnableClickEvents = false;
            PC->bEnableMouseOverEvents = false;
            PC->CurrentMouseCursor = EMouseCursor::Default;
        }
    }

    RegisterEnhancedInputMappingContext();

    if (UGameInstance* GI = GetGameInstance())
    {
        if (UNazareneSettingsSubsystem* Settings = GI->GetSubsystem<UNazareneSettingsSubsystem>())
        {
            const FNazarenePlayerSettings& Data = Settings->GetSettings();
            ApplyUserLookAndCameraSettings(Data.MouseSensitivity, Data.bInvertLookY, Data.FieldOfView);
        }
    }

    SmoothedMoveForwardAxis = MoveForwardAxis;
    SmoothedMoveRightAxis = MoveRightAxis;
    UpdateCameraState(0.0f);
}

void ANazarenePlayerCharacter::ConfigureProxyVisuals()
{
    UMaterialInterface* ShapeMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    if (ShapeMaterial == nullptr)
    {
        return;
    }

    const auto ApplyTint = [ShapeMaterial](UStaticMeshComponent* Component, const FLinearColor& Tint)
    {
        if (Component == nullptr)
        {
            return;
        }
        Component->SetMaterial(0, ShapeMaterial);
        Component->SetVectorParameterValueOnMaterials(TEXT("Color"), FVector(Tint.R, Tint.G, Tint.B));
    };

    ApplyTint(BodyMesh, FLinearColor(0.73f, 0.70f, 0.62f));
    ApplyTint(HeadMesh, FLinearColor(0.70f, 0.56f, 0.42f));
    ApplyTint(MantleMesh, FLinearColor(0.80f, 0.72f, 0.56f));
    ApplyTint(RobeMesh, FLinearColor(0.59f, 0.49f, 0.35f));
    ApplyTint(SashMesh, FLinearColor(0.90f, 0.86f, 0.74f));
    ApplyTint(StaffMesh, FLinearColor(0.38f, 0.27f, 0.18f));
}

void ANazarenePlayerCharacter::SetProxyVisualsHidden(bool bHideProxy)
{
    if (BodyMesh != nullptr)
    {
        BodyMesh->SetHiddenInGame(bHideProxy);
        BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    if (HeadMesh != nullptr)
    {
        HeadMesh->SetHiddenInGame(bHideProxy);
        HeadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    if (MantleMesh != nullptr)
    {
        MantleMesh->SetHiddenInGame(bHideProxy);
        MantleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    if (RobeMesh != nullptr)
    {
        RobeMesh->SetHiddenInGame(bHideProxy);
        RobeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    if (SashMesh != nullptr)
    {
        SashMesh->SetHiddenInGame(bHideProxy);
        SashMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    if (StaffMesh != nullptr)
    {
        StaffMesh->SetHiddenInGame(bHideProxy);
        StaffMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}

void ANazarenePlayerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    UpdateTimers(DeltaSeconds);
    RegenStamina(DeltaSeconds);
    ValidateLockTarget();
    UpdateMovementState(DeltaSeconds);
    UpdateCameraState(DeltaSeconds);

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
    if (InputMappingContextAsset != nullptr)
    {
        RuntimeInputMappingContext = InputMappingContextAsset;

        auto FindMappedAction = [this](const TCHAR* Token) -> UInputAction*
        {
            if (RuntimeInputMappingContext == nullptr)
            {
                return nullptr;
            }

            for (const FEnhancedActionKeyMapping& Mapping : RuntimeInputMappingContext->GetMappings())
            {
                const UInputAction* Action = Mapping.Action.Get();
                if (Action != nullptr && Action->GetName().Contains(Token))
                {
                    return const_cast<UInputAction*>(Action);
                }
            }
            return nullptr;
        };

        MoveForwardInputAction = FindMappedAction(TEXT("MoveForward"));
        MoveRightInputAction = FindMappedAction(TEXT("MoveRight"));
        TurnInputAction = FindMappedAction(TEXT("Turn"));
        LookUpInputAction = FindMappedAction(TEXT("LookUp"));
        InteractInputAction = FindMappedAction(TEXT("Interact"));
        LockOnInputAction = FindMappedAction(TEXT("LockOn"));
        PauseInputAction = FindMappedAction(TEXT("Pause"));
        ToggleMouseCaptureInputAction = FindMappedAction(TEXT("ToggleMouseCapture"));
        BlockInputAction = FindMappedAction(TEXT("Block"));
        LightAttackInputAction = FindMappedAction(TEXT("LightAttack"));
        HeavyAttackInputAction = FindMappedAction(TEXT("HeavyAttack"));
        DodgeInputAction = FindMappedAction(TEXT("Dodge"));
        ParryInputAction = FindMappedAction(TEXT("Parry"));
        MiracleHealInputAction = FindMappedAction(TEXT("MiracleHeal"));
        MiracleBlessingInputAction = FindMappedAction(TEXT("MiracleBlessing"));
        MiracleRadianceInputAction = FindMappedAction(TEXT("MiracleRadiance"));
        SaveSlot1InputAction = FindMappedAction(TEXT("SaveSlot1"));
        SaveSlot2InputAction = FindMappedAction(TEXT("SaveSlot2"));
        SaveSlot3InputAction = FindMappedAction(TEXT("SaveSlot3"));
        LoadSlot1InputAction = FindMappedAction(TEXT("LoadSlot1"));
        LoadSlot2InputAction = FindMappedAction(TEXT("LoadSlot2"));
        LoadSlot3InputAction = FindMappedAction(TEXT("LoadSlot3"));
        SkillTreeInputAction = FindMappedAction(TEXT("SkillTree"));

        const bool bHasAllAssetActions =
            MoveForwardInputAction != nullptr &&
            MoveRightInputAction != nullptr &&
            TurnInputAction != nullptr &&
            LookUpInputAction != nullptr &&
            InteractInputAction != nullptr &&
            LockOnInputAction != nullptr &&
            PauseInputAction != nullptr &&
            ToggleMouseCaptureInputAction != nullptr &&
            BlockInputAction != nullptr &&
            LightAttackInputAction != nullptr &&
            HeavyAttackInputAction != nullptr &&
            DodgeInputAction != nullptr &&
            ParryInputAction != nullptr &&
            MiracleHealInputAction != nullptr &&
            MiracleBlessingInputAction != nullptr &&
            MiracleRadianceInputAction != nullptr &&
            SaveSlot1InputAction != nullptr &&
            SaveSlot2InputAction != nullptr &&
            SaveSlot3InputAction != nullptr &&
            LoadSlot1InputAction != nullptr &&
            LoadSlot2InputAction != nullptr &&
            LoadSlot3InputAction != nullptr;

        if (bHasAllAssetActions)
        {
            return;
        }

        UE_LOG(LogTemp, Warning, TEXT("NazarenePlayerCharacter asset mapping context is missing required actions. Falling back to runtime defaults."));
        RuntimeInputMappingContext = nullptr;
    }

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
    SkillTreeInputAction = MakeInputAction(this, TEXT("NazareneSkillTree"), EInputActionValueType::Boolean);

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

    AddScaledMapping(RuntimeInputMappingContext, MoveForwardInputAction, EKeys::Gamepad_LeftY, 1.0f);
    AddScaledMapping(RuntimeInputMappingContext, MoveRightInputAction, EKeys::Gamepad_LeftX, 1.0f);
    AddScaledMapping(RuntimeInputMappingContext, TurnInputAction, EKeys::Gamepad_RightX, 1.8f);
    AddScaledMapping(RuntimeInputMappingContext, LookUpInputAction, EKeys::Gamepad_RightY, -1.6f);

    RuntimeInputMappingContext->MapKey(InteractInputAction, EKeys::E);
    RuntimeInputMappingContext->MapKey(InteractInputAction, EKeys::Gamepad_FaceButton_Bottom);
    RuntimeInputMappingContext->MapKey(LockOnInputAction, EKeys::Q);
    RuntimeInputMappingContext->MapKey(LockOnInputAction, EKeys::Gamepad_RightThumbstick);
    RuntimeInputMappingContext->MapKey(PauseInputAction, EKeys::Escape);
    RuntimeInputMappingContext->MapKey(PauseInputAction, EKeys::Gamepad_Special_Right);
    RuntimeInputMappingContext->MapKey(ToggleMouseCaptureInputAction, EKeys::Tab);
    RuntimeInputMappingContext->MapKey(BlockInputAction, EKeys::LeftShift);
    RuntimeInputMappingContext->MapKey(BlockInputAction, EKeys::Gamepad_LeftTriggerAxis);
    RuntimeInputMappingContext->MapKey(LightAttackInputAction, EKeys::LeftMouseButton);
    RuntimeInputMappingContext->MapKey(LightAttackInputAction, EKeys::Gamepad_FaceButton_Left);
    RuntimeInputMappingContext->MapKey(HeavyAttackInputAction, EKeys::RightMouseButton);
    RuntimeInputMappingContext->MapKey(HeavyAttackInputAction, EKeys::Gamepad_RightTriggerAxis);
    RuntimeInputMappingContext->MapKey(DodgeInputAction, EKeys::SpaceBar);
    RuntimeInputMappingContext->MapKey(DodgeInputAction, EKeys::Gamepad_FaceButton_Right);
    RuntimeInputMappingContext->MapKey(ParryInputAction, EKeys::F);
    RuntimeInputMappingContext->MapKey(ParryInputAction, EKeys::Gamepad_RightShoulder);
    RuntimeInputMappingContext->MapKey(MiracleHealInputAction, EKeys::R);
    RuntimeInputMappingContext->MapKey(MiracleHealInputAction, EKeys::Gamepad_LeftShoulder);
    RuntimeInputMappingContext->MapKey(MiracleBlessingInputAction, EKeys::One);
    RuntimeInputMappingContext->MapKey(MiracleBlessingInputAction, EKeys::Gamepad_DPad_Up);
    RuntimeInputMappingContext->MapKey(MiracleRadianceInputAction, EKeys::Two);
    RuntimeInputMappingContext->MapKey(MiracleRadianceInputAction, EKeys::Gamepad_FaceButton_Top);

    RuntimeInputMappingContext->MapKey(SaveSlot1InputAction, EKeys::F1);
    RuntimeInputMappingContext->MapKey(SaveSlot2InputAction, EKeys::F2);
    RuntimeInputMappingContext->MapKey(SaveSlot3InputAction, EKeys::F3);
    RuntimeInputMappingContext->MapKey(LoadSlot1InputAction, EKeys::F5);
    RuntimeInputMappingContext->MapKey(LoadSlot2InputAction, EKeys::F6);
    RuntimeInputMappingContext->MapKey(LoadSlot3InputAction, EKeys::F7);
    RuntimeInputMappingContext->MapKey(SkillTreeInputAction, EKeys::T);
    RuntimeInputMappingContext->MapKey(SkillTreeInputAction, EKeys::Gamepad_DPad_Left);
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
    EnhancedInputComponent->BindAction(SkillTreeInputAction, ETriggerEvent::Started, this, &ANazarenePlayerCharacter::TryToggleSkillTree);
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
    if (bBabyIntroMode || !bCombatEnabled)
    {
        return;
    }

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
    if (AttributeSet != nullptr)
    {
        AttributeSet->SetFaith(CurrentFaith);
    }
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

void ANazarenePlayerCharacter::SetCombatEnabled(bool bEnabled)
{
    bCombatEnabled = bEnabled;

    if (!bCombatEnabled)
    {
        bIsBlocking = false;
        LockTarget.Reset();
        PendingAttack = ENazarenePlayerAttackType::None;
        AttackWindupTimer = 0.0f;
        AttackActiveTimer = 0.0f;
        bAttackResolved = false;
        AttackCooldown = 0.0f;
        ParryWindowTimer = 0.0f;
        ParryStartupTimer = 0.0f;
    }
}

void ANazarenePlayerCharacter::SetBabyIntroMode(bool bEnabled)
{
    if (bBabyIntroMode == bEnabled)
    {
        return;
    }

    bBabyIntroMode = bEnabled;
    if (bBabyIntroMode)
    {
        DefaultActorScale = GetActorScale3D();
        const float IntroScale = FMath::Max(0.1f, BabyIntroVisualScale);
        SetActorScale3D(FVector(IntroScale, IntroScale, IntroScale));

        bIsBlocking = false;
        LockTarget.Reset();
        PendingAttack = ENazarenePlayerAttackType::None;
        AttackWindupTimer = 0.0f;
        AttackActiveTimer = 0.0f;
        bAttackResolved = false;
        AttackCooldown = FMath::Max(AttackCooldown, 0.2f);
        DodgeTimer = 0.0f;
        InvulnerabilityTimer = 0.0f;
        ParryWindowTimer = 0.0f;
        ParryStartupTimer = 0.0f;
        if (StaffMesh != nullptr)
        {
            StaffMesh->SetHiddenInGame(true);
        }
    }
    else
    {
        SetActorScale3D(DefaultActorScale);
        if (StaffMesh != nullptr)
        {
            const bool bCharacterMeshVisible = GetMesh() != nullptr && GetMesh()->GetSkeletalMeshAsset() != nullptr;
            StaffMesh->SetHiddenInGame(bCharacterMeshVisible);
        }
    }
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

void ANazarenePlayerCharacter::ApplyUserLookAndCameraSettings(float InMouseSensitivity, bool bInInvertLookY, float InFieldOfView)
{
    MouseSensitivityScale = FMath::Clamp(InMouseSensitivity, 0.2f, 3.0f);
    bInvertLookY = bInInvertLookY;
    BaseCameraFOV = FMath::Clamp(InFieldOfView, 60.0f, 110.0f);

    if (FollowCamera != nullptr)
    {
        FollowCamera->SetFieldOfView(BaseCameraFOV);
    }
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

bool ANazarenePlayerCharacter::IsBlocking() const
{
    return bIsBlocking;
}

bool ANazarenePlayerCharacter::IsDodging() const
{
    return DodgeTimer > 0.0f;
}

bool ANazarenePlayerCharacter::IsAttacking() const
{
    return PendingAttack != ENazarenePlayerAttackType::None || AttackCooldown > 0.0f;
}

bool ANazarenePlayerCharacter::HasLockTarget() const
{
    return LockTarget.IsValid();
}

bool ANazarenePlayerCharacter::IsDefeated() const
{
    return CurrentHealth <= 0.01f;
}

float ANazarenePlayerCharacter::GetHurtTimeRemaining() const
{
    return HurtTimer;
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

ANazareneEnemyCharacter* ANazarenePlayerCharacter::GetLockTargetActor() const
{
    return LockTarget.Get();
}

int32 ANazarenePlayerCharacter::XPForLevel(int32 LevelValue)
{
    const int32 SafeLevel = FMath::Max(LevelValue, 1);
    return 45 + (SafeLevel - 1) * (SafeLevel - 1) * 28;
}

int32 ANazarenePlayerCharacter::GetXPToNextLevel() const
{
    return FMath::Max(XPForLevel(PlayerLevel + 1) - TotalXP, 0);
}

void ANazarenePlayerCharacter::SetSkillTreeState(const TArray<FName>& Skills, int32 InSkillPoints, int32 InTotalXP, int32 InPlayerLevel)
{
    UnlockedSkills = Skills;
    UnspentSkillPoints = FMath::Max(0, InSkillPoints);
    TotalXP = FMath::Max(0, InTotalXP);
    PlayerLevel = FMath::Max(1, InPlayerLevel);
    ApplySkillModifiers();
}

bool ANazarenePlayerCharacter::AttemptUnlockSkill(FName SkillId)
{
    if (!UNazareneSkillTree::CanUnlockSkill(SkillId, UnlockedSkills, TotalXP, UnspentSkillPoints))
    {
        SetContextHint(TEXT("Skill requirements not met."));
        return false;
    }

    FNazareneSkillDefinition Definition;
    if (!UNazareneSkillTree::GetSkillDefinition(SkillId, Definition))
    {
        SetContextHint(TEXT("Skill definition missing."));
        return false;
    }

    UnlockedSkills.AddUnique(SkillId);
    UnspentSkillPoints = FMath::Max(0, UnspentSkillPoints - FMath::Max(Definition.Cost, 1));
    ApplySkillModifiers();
    SetContextHint(FString::Printf(TEXT("Unlocked skill: %s"), *Definition.Name));
    return true;
}

void ANazarenePlayerCharacter::SetCampaignBaseVitals(float InMaxHealth, float InMaxStamina, bool bRestoreToFull)
{
    CampaignBaseMaxHealth = FMath::Max(1.0f, InMaxHealth);
    CampaignBaseMaxStamina = FMath::Max(1.0f, InMaxStamina);
    ApplySkillModifiers();

    if (bRestoreToFull)
    {
        CurrentHealth = MaxHealth;
        CurrentStamina = MaxStamina;
    }
    else
    {
        CurrentHealth = FMath::Min(CurrentHealth, MaxHealth);
        CurrentStamina = FMath::Min(CurrentStamina, MaxStamina);
    }
}

void ANazarenePlayerCharacter::DisplayDamageNumber(const FVector& WorldLocation, float Amount, ENazareneDamageNumberType Type) const
{
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (ANazareneHUD* HUD = Cast<ANazareneHUD>(PC->GetHUD()))
        {
            HUD->ShowDamageNumber(WorldLocation, Amount, Type);
        }
    }
}

void ANazarenePlayerCharacter::ApplySkillModifiers()
{
    LightAttackDamage = 26.0f;
    HeavyAttackDamage = 42.0f;
    HeavyAttackPoiseDamage = 54.0f;
    HeavyAttackRange = 340.0f;
    WalkSpeed = 580.0f;
    DodgeStaminaCost = 26.0f;
    HealAmount = 45.0f;
    RadianceDamage = 32.0f;
    RadianceRadius = 600.0f;
    StaminaRegen = 22.0f;

    float SkillHealthBonus = 0.0f;
    float SkillStaminaBonus = 0.0f;

    if (UnlockedSkills.Contains(FName(TEXT("combat_smite"))))
    {
        LightAttackDamage *= 1.1f;
        HeavyAttackDamage *= 1.1f;
    }
    if (UnlockedSkills.Contains(FName(TEXT("combat_crusader"))))
    {
        HeavyAttackPoiseDamage *= 1.12f;
        HeavyAttackRange += 40.0f;
    }
    if (UnlockedSkills.Contains(FName(TEXT("movement_pilgrim_stride"))))
    {
        WalkSpeed *= 1.12f;
    }
    if (UnlockedSkills.Contains(FName(TEXT("movement_swift_vow"))))
    {
        DodgeStaminaCost *= 0.82f;
    }
    if (UnlockedSkills.Contains(FName(TEXT("miracles_abundance"))))
    {
        HealAmount *= 1.18f;
    }
    if (UnlockedSkills.Contains(FName(TEXT("miracles_radiance_lance"))))
    {
        RadianceDamage *= 1.2f;
        RadianceRadius += 120.0f;
    }
    if (UnlockedSkills.Contains(FName(TEXT("defense_shepherd_guard"))))
    {
        SkillHealthBonus += 14.0f;
    }
    if (UnlockedSkills.Contains(FName(TEXT("defense_steadfast"))))
    {
        SkillStaminaBonus += 18.0f;
        StaminaRegen *= 1.15f;
    }

    MaxHealth = CampaignBaseMaxHealth + SkillHealthBonus;
    MaxStamina = CampaignBaseMaxStamina + SkillStaminaBonus;
    CurrentHealth = FMath::Min(CurrentHealth, MaxHealth);
    CurrentStamina = FMath::Min(CurrentStamina, MaxStamina);

    if (GetCharacterMovement() != nullptr)
    {
        GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    }

    if (AttributeSet != nullptr)
    {
        AttributeSet->SetMaxHealth(MaxHealth);
        AttributeSet->SetHealth(CurrentHealth);
        AttributeSet->SetMaxStamina(MaxStamina);
        AttributeSet->SetStamina(CurrentStamina);
        AttributeSet->SetFaith(CurrentFaith);
    }
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

    if (AttributeSet != nullptr)
    {
        AttributeSet->SetHealth(CurrentHealth);
        AttributeSet->SetStamina(CurrentStamina);
        AttributeSet->SetFaith(CurrentFaith);
    }
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

    if (AttributeSet != nullptr)
    {
        AttributeSet->SetHealth(CurrentHealth);
        AttributeSet->SetStamina(CurrentStamina);
    }

    // Faith refill at prayer site
    const float FaithRefill = StartingFaith * Site->FaithRefillPercent;
    CurrentFaith = FMath::Min(CurrentFaith + FaithRefill, StartingFaith * 2.0f);

    // Show lore text if available
    if (!Site->LoreText.IsEmpty())
    {
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            if (ANazareneHUD* HUD = Cast<ANazareneHUD>(PC->GetHUD()))
            {
                HUD->ShowMessage(Site->LoreText, 6.0f);
            }
        }
    }

    SetContextHint(Site->GetPromptMessage());
    if (CampaignGameMode.IsValid())
    {
        CampaignGameMode->NotifyPrayerSiteRest(Site->SiteId);
    }

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

void ANazarenePlayerCharacter::SetActiveNPC(ANazareneNPC* NPC)
{
    ActiveNPC = NPC;
}

void ANazarenePlayerCharacter::ClearActiveNPC(ANazareneNPC* NPC)
{
    if (ActiveNPC.Get() == NPC)
    {
        ActiveNPC.Reset();
    }
}

void ANazarenePlayerCharacter::MoveForward(float Value)
{
    const float ClampedValue = FMath::Clamp(Value, -1.0f, 1.0f);
    MoveForwardAxis = (FMath::Abs(ClampedValue) >= MoveInputDeadZone) ? ClampedValue : 0.0f;
}

void ANazarenePlayerCharacter::MoveRight(float Value)
{
    const float ClampedValue = FMath::Clamp(Value, -1.0f, 1.0f);
    MoveRightAxis = (FMath::Abs(ClampedValue) >= MoveInputDeadZone) ? ClampedValue : 0.0f;
}

void ANazarenePlayerCharacter::Turn(float Value)
{
    AddControllerYawInput(Value * MouseSensitivityScale);
}

void ANazarenePlayerCharacter::LookUp(float Value)
{
    const float InvertScale = bInvertLookY ? -1.0f : 1.0f;
    AddControllerPitchInput(Value * MouseSensitivityScale * InvertScale);
}

void ANazarenePlayerCharacter::Interact()
{
    if (ActivePrayerSite.IsValid())
    {
        RestAtPrayerSite(ActivePrayerSite.Get());
        return;
    }

    if (ActiveNPC.IsValid())
    {
        ActiveNPC->AdvanceDialogue();
        if (CampaignGameMode.IsValid())
        {
            const FString Slug = ActiveNPC->CharacterSlug;
            if (!Slug.IsEmpty())
            {
                CampaignGameMode->NotifyNPCDialogue(FName(*Slug.ToLower()));
            }
        }
        return;
    }

    if (ActiveTravelGate.IsValid())
    {
        ActiveTravelGate->RequestTravel();
    }
}

void ANazarenePlayerCharacter::ToggleLockOn()
{
    if (bBabyIntroMode || !bCombatEnabled)
    {
        return;
    }

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
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC == nullptr)
    {
        return;
    }

    ANazareneHUD* HUD = Cast<ANazareneHUD>(PC->GetHUD());
    if (HUD != nullptr)
    {
        HUD->TogglePauseMenu();
        return;
    }

    const bool bPaused = UGameplayStatics::IsGamePaused(this);
    UGameplayStatics::SetGamePaused(this, !bPaused);

    if (!bPaused)
    {
        PC->bShowMouseCursor = true;
        PC->bEnableClickEvents = true;
        PC->bEnableMouseOverEvents = true;
        PC->CurrentMouseCursor = EMouseCursor::None;
        PC->SetInputMode(FInputModeGameAndUI());
    }
    else
    {
        PC->bShowMouseCursor = false;
        PC->bEnableClickEvents = false;
        PC->bEnableMouseOverEvents = false;
        PC->CurrentMouseCursor = EMouseCursor::Default;
        PC->SetInputMode(FInputModeGameOnly());
    }
}

void ANazarenePlayerCharacter::ToggleMouseCapture()
{
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        const bool bShow = !PC->bShowMouseCursor;
        PC->bShowMouseCursor = bShow;
        PC->bEnableClickEvents = bShow;
        PC->bEnableMouseOverEvents = bShow;
        PC->CurrentMouseCursor = bShow ? EMouseCursor::None : EMouseCursor::Default;
        if (bShow)
        {
            FInputModeGameAndUI InputMode;
            InputMode.SetHideCursorDuringCapture(false);
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PC->SetInputMode(InputMode);
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
    if (bBabyIntroMode || !bCombatEnabled)
    {
        return;
    }

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
    TriggerPresentation(LightAttackSound, LightAttackVFX, GetActorLocation() + GetActorForwardVector() * 110.0f, 0.85f);
}

void ANazarenePlayerCharacter::TryHeavyAttack()
{
    if (bBabyIntroMode || !bCombatEnabled)
    {
        return;
    }

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
    TriggerPresentation(HeavyAttackSound, HeavyAttackVFX, GetActorLocation() + GetActorForwardVector() * 125.0f, 0.95f);
}

void ANazarenePlayerCharacter::TryDodge()
{
    if (bBabyIntroMode || !bCombatEnabled)
    {
        return;
    }

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
        const FVector InputDir = Forward * SmoothedMoveForwardAxis + Right * SmoothedMoveRightAxis;
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
    TriggerPresentation(DodgeSound, DodgeVFX, GetActorLocation(), 0.8f);
}

void ANazarenePlayerCharacter::TryParry()
{
    if (bBabyIntroMode || !bCombatEnabled)
    {
        return;
    }

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
    TriggerPresentation(HeavyAttackSound, nullptr, GetActorLocation(), 0.45f);
}

void ANazarenePlayerCharacter::TryHealingMiracle()
{
    if (bBabyIntroMode || !bCombatEnabled)
    {
        return;
    }

    if (AbilitySystemComponent != nullptr)
    {
        if (AbilitySystemComponent->TryActivateAbilityByClass(UGA_NazareneHeal::StaticClass()))
        {
            // GAS ability handles faith deduction and healing;
            // sync the local state from the AttributeSet back to the character
            if (AttributeSet != nullptr)
            {
                CurrentHealth = AttributeSet->GetHealth();
                CurrentFaith = AttributeSet->GetFaith();
            }
            HealCooldownTimer = HealCooldown;
            AttackCooldown = FMath::Max(AttackCooldown, 0.35f);
            TriggerPresentation(MiracleSound, MiracleVFX, GetActorLocation());
        }
    }
}

void ANazarenePlayerCharacter::TryBlessingMiracle()
{
    if (bBabyIntroMode || !bCombatEnabled)
    {
        return;
    }

    if (AbilitySystemComponent != nullptr)
    {
        if (AbilitySystemComponent->TryActivateAbilityByClass(UGA_NazareneBlessing::StaticClass()))
        {
            // GAS ability handles faith deduction;
            // sync local state and set timers
            if (AttributeSet != nullptr)
            {
                CurrentFaith = AttributeSet->GetFaith();
            }
            BlessingTimer = BlessingDuration;
            BlessingCooldownTimer = BlessingCooldown;
            AttackCooldown = FMath::Max(AttackCooldown, 0.35f);
            TriggerPresentation(MiracleSound, MiracleVFX, GetActorLocation());
        }
    }
}

void ANazarenePlayerCharacter::TryRadianceMiracle()
{
    if (bBabyIntroMode || !bCombatEnabled)
    {
        return;
    }

    if (AbilitySystemComponent != nullptr)
    {
        if (AbilitySystemComponent->TryActivateAbilityByClass(UGA_NazareneRadiance::StaticClass()))
        {
            // GAS ability handles faith deduction and AOE damage;
            // sync local state and set timers
            if (AttributeSet != nullptr)
            {
                CurrentFaith = AttributeSet->GetFaith();
            }
            RadianceCooldownTimer = RadianceCooldown;
            AttackCooldown = FMath::Max(AttackCooldown, 0.45f);
            TriggerPresentation(MiracleSound, MiracleVFX, GetActorLocation());
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

void ANazarenePlayerCharacter::TryUnlockFirstAvailableSkill()
{
    for (const FName SkillId : UNazareneSkillTree::GetAllSkillIds())
    {
        if (AttemptUnlockSkill(SkillId))
        {
            return;
        }
    }
}

void ANazarenePlayerCharacter::TryToggleSkillTree()
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC == nullptr)
    {
        return;
    }

    ANazareneHUD* HUD = Cast<ANazareneHUD>(PC->GetHUD());
    if (HUD != nullptr)
    {
        HUD->ToggleSkillTree();
    }
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
    if (AttributeSet != nullptr)
    {
        AttributeSet->SetStamina(CurrentStamina);
    }
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
            DisplayDamageNumber(Enemy->GetActorLocation() + FVector(0.0f, 0.0f, 130.0f), HeavyAttackDamage * 1.08f, ENazareneDamageNumberType::Critical);
        }
        else
        {
            Enemy->ReceiveCombatHit(LightAttackDamage, LightAttackPoiseDamage, this);
            AddFaith(1.0f);
            DisplayDamageNumber(Enemy->GetActorLocation() + FVector(0.0f, 0.0f, 130.0f), LightAttackDamage, ENazareneDamageNumberType::Normal);
        }
    }
    else
    {
        if (Enemy->IsParried())
        {
            Enemy->ReceiveRiposte(HeavyAttackDamage * 1.5f, this);
            DisplayDamageNumber(Enemy->GetActorLocation() + FVector(0.0f, 0.0f, 130.0f), HeavyAttackDamage * 1.5f, ENazareneDamageNumberType::Critical);
        }
        else
        {
            Enemy->ReceiveCombatHit(HeavyAttackDamage, HeavyAttackPoiseDamage, this);
            AddFaith(1.5f);
            DisplayDamageNumber(Enemy->GetActorLocation() + FVector(0.0f, 0.0f, 130.0f), HeavyAttackDamage, ENazareneDamageNumberType::Normal);
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
    return bBabyIntroMode || !bCombatEnabled || AttackCooldown > 0.0f || DodgeTimer > 0.0f || HurtTimer > 0.0f;
}

bool ANazarenePlayerCharacter::ConsumeStamina(float Cost)
{
    if (CurrentStamina < Cost)
    {
        return false;
    }
    CurrentStamina -= Cost;
    if (AttributeSet != nullptr)
    {
        AttributeSet->SetStamina(CurrentStamina);
    }
    return true;
}

void ANazarenePlayerCharacter::ApplyHealthDamage(float Amount)
{
    CurrentHealth = FMath::Max(0.0f, CurrentHealth - Amount);
    if (AttributeSet != nullptr)
    {
        AttributeSet->SetHealth(CurrentHealth);
    }
    HurtTimer = 0.22f;
    TriggerPresentation(HurtSound, HurtVFX, GetActorLocation());
    if (CurrentHealth <= 0.01f)
    {
        HandleDefeat();
    }
}

void ANazarenePlayerCharacter::HandleDefeat()
{
    if (CampaignGameMode.IsValid())
    {
        CampaignGameMode->NotifyPlayerDefeated();
    }

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
        if (AttributeSet != nullptr)
        {
            AttributeSet->SetHealth(CurrentHealth);
            AttributeSet->SetStamina(CurrentStamina);
        }
    }
}

void ANazarenePlayerCharacter::UpdateMovementState(float DeltaSeconds)
{
    bIsBlocking = bIsBlocking && DodgeTimer <= 0.0f && PendingAttack == ENazarenePlayerAttackType::None;

    const float SmoothingSpeed = FMath::Max(0.0f, MoveInputSmoothingSpeed);
    SmoothedMoveForwardAxis = FMath::FInterpTo(SmoothedMoveForwardAxis, MoveForwardAxis, DeltaSeconds, SmoothingSpeed);
    SmoothedMoveRightAxis = FMath::FInterpTo(SmoothedMoveRightAxis, MoveRightAxis, DeltaSeconds, SmoothingSpeed);
    const float SmoothedInputMagnitude = FMath::Clamp(FVector2D(SmoothedMoveForwardAxis, SmoothedMoveRightAxis).Size(), 0.0f, 1.5f);
    const bool bHasValidLockTarget = LockTarget.IsValid() && !LockTarget->IsRedeemed();
    const bool bCombatState = bHasValidLockTarget || bIsBlocking || PendingAttack != ENazarenePlayerAttackType::None || HurtTimer > 0.0f;

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
    if (!bCombatState && SmoothedInputMagnitude >= TraversalInputThreshold)
    {
        MovementMultiplier = FMath::Max(MovementMultiplier, TraversalRunMultiplier);
    }
    if (bBabyIntroMode)
    {
        MovementMultiplier *= FMath::Clamp(BabyIntroMovementMultiplier, 0.2f, 1.0f);
    }

    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed * MovementMultiplier;

    if (AController* C = GetController())
    {
        if (bHasValidLockTarget)
        {
            const FRotator ActorYawRotation(0.0f, GetActorRotation().Yaw, 0.0f);
            const FVector Forward = FRotationMatrix(ActorYawRotation).GetUnitAxis(EAxis::X);
            const FVector Right = FRotationMatrix(ActorYawRotation).GetUnitAxis(EAxis::Y);

            if (FMath::Abs(SmoothedMoveForwardAxis) > KINDA_SMALL_NUMBER)
            {
                const float ForwardScale = SmoothedMoveForwardAxis >= 0.0f ? LockOnForwardSpeedMultiplier : LockOnBackpedalSpeedMultiplier;
                AddMovementInput(Forward, SmoothedMoveForwardAxis * ForwardScale);
            }
            if (FMath::Abs(SmoothedMoveRightAxis) > KINDA_SMALL_NUMBER)
            {
                AddMovementInput(Right, SmoothedMoveRightAxis * LockOnStrafeSpeedMultiplier);
            }
        }
        else
        {
            const FRotator YawRotation(0.0f, C->GetControlRotation().Yaw, 0.0f);
            const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
            const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
            if (FMath::Abs(SmoothedMoveForwardAxis) > KINDA_SMALL_NUMBER)
            {
                AddMovementInput(Forward, SmoothedMoveForwardAxis);
            }
            if (FMath::Abs(SmoothedMoveRightAxis) > KINDA_SMALL_NUMBER)
            {
                AddMovementInput(Right, SmoothedMoveRightAxis);
            }
        }
    }

    GetCharacterMovement()->bOrientRotationToMovement = !bHasValidLockTarget;

    if (bHasValidLockTarget)
    {
        FVector ToTarget = LockTarget->GetActorLocation() - GetActorLocation();
        ToTarget.Z = 0.0f;
        if (!ToTarget.IsNearlyZero())
        {
            const FRotator Desired = ToTarget.Rotation();
            const FRotator NewRot = FMath::RInterpTo(GetActorRotation(), Desired, DeltaSeconds, LockOnRotationInterpSpeed);
            SetActorRotation(FRotator(0.0f, NewRot.Yaw, 0.0f));
        }
    }
}

void ANazarenePlayerCharacter::UpdateCameraState(float DeltaSeconds)
{
    if (CameraBoom == nullptr || FollowCamera == nullptr)
    {
        return;
    }

    const bool bHasValidLockTarget = LockTarget.IsValid() && !LockTarget->IsRedeemed();
    const bool bCombatState = bHasValidLockTarget || bIsBlocking || PendingAttack != ENazarenePlayerAttackType::None || HurtTimer > 0.0f;
    const float CurrentSpeed = GetVelocity().Size2D();
    const float SpeedAlpha = FMath::Clamp(CurrentSpeed / FMath::Max(WalkSpeed, 1.0f), 0.0f, 1.35f);

    const float TargetArmLength = bHasValidLockTarget ? LockOnCameraArmLength : FreeCameraArmLength;
    const float TargetFOV = FMath::Clamp(
        BaseCameraFOV + (SpeedAlpha * SpeedFOVBoost) - (bCombatState ? CombatFOVPenalty : 0.0f),
        60.0f,
        110.0f);

    if (DeltaSeconds <= KINDA_SMALL_NUMBER)
    {
        CameraBoom->TargetArmLength = TargetArmLength;
        FollowCamera->SetFieldOfView(TargetFOV);
        return;
    }

    CameraBoom->TargetArmLength = FMath::FInterpTo(
        CameraBoom->TargetArmLength,
        TargetArmLength,
        DeltaSeconds,
        FMath::Max(0.01f, CameraArmInterpSpeed));

    FollowCamera->SetFieldOfView(FMath::FInterpTo(
        FollowCamera->FieldOfView,
        TargetFOV,
        DeltaSeconds,
        FMath::Max(0.01f, CameraFOVInterpSpeed)));
}

void ANazarenePlayerCharacter::TriggerPresentation(USoundBase* Sound, UNiagaraSystem* Effect, const FVector& Location, float VolumeMultiplier) const
{
    if (Sound != nullptr)
    {
        UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation(), FRotator::ZeroRotator, VolumeMultiplier);
    }

    if (Effect != nullptr && GetWorld() != nullptr)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), Effect, Location, GetActorRotation());
    }
}
