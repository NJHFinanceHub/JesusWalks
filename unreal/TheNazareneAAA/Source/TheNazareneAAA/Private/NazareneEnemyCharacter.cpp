#include "NazareneEnemyCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NazareneAssetResolver.h"
#include "NazareneEnemyAIController.h"
#include "NazareneEnemyAnimInstance.h"
#include "NazarenePlayerCharacter.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
    const TCHAR* EnemyMeshOverrideKey(ENazareneEnemyArchetype Archetype)
    {
        switch (Archetype)
        {
        case ENazareneEnemyArchetype::MeleeShield: return TEXT("EnemySkeletalMesh_MeleeShield");
        case ENazareneEnemyArchetype::Spear: return TEXT("EnemySkeletalMesh_Spear");
        case ENazareneEnemyArchetype::Ranged: return TEXT("EnemySkeletalMesh_Ranged");
        case ENazareneEnemyArchetype::Demon: return TEXT("EnemySkeletalMesh_Demon");
        case ENazareneEnemyArchetype::Boss: return TEXT("EnemySkeletalMesh_Boss");
        default: break;
        }
        return nullptr;
    }

    const TCHAR* EnemyAnimOverrideKey(ENazareneEnemyArchetype Archetype)
    {
        switch (Archetype)
        {
        case ENazareneEnemyArchetype::MeleeShield: return TEXT("EnemyAnimBlueprint_MeleeShield");
        case ENazareneEnemyArchetype::Spear: return TEXT("EnemyAnimBlueprint_Spear");
        case ENazareneEnemyArchetype::Ranged: return TEXT("EnemyAnimBlueprint_Ranged");
        case ENazareneEnemyArchetype::Demon: return TEXT("EnemyAnimBlueprint_Demon");
        case ENazareneEnemyArchetype::Boss: return TEXT("EnemyAnimBlueprint_Boss");
        default: break;
        }
        return nullptr;
    }
}

ANazareneEnemyCharacter::ANazareneEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
    BodyMesh->SetupAttachment(GetCapsuleComponent());
    BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -54.0f));
    BodyMesh->SetRelativeScale3D(FVector(0.48f, 0.48f, 1.18f));
    BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
    HeadMesh->SetupAttachment(GetCapsuleComponent());
    HeadMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 22.0f));
    HeadMesh->SetRelativeScale3D(FVector(0.33f, 0.33f, 0.35f));
    HeadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    MantleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MantleMesh"));
    MantleMesh->SetupAttachment(GetCapsuleComponent());
    MantleMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -22.0f));
    MantleMesh->SetRelativeScale3D(FVector(0.75f, 0.46f, 0.2f));
    MantleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    RobeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RobeMesh"));
    RobeMesh->SetupAttachment(GetCapsuleComponent());
    RobeMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
    RobeMesh->SetRelativeScale3D(FVector(0.75f, 0.75f, 1.58f));
    RobeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->SetupAttachment(GetCapsuleComponent());
    WeaponMesh->SetRelativeLocation(FVector(26.0f, 18.0f, -40.0f));
    WeaponMesh->SetRelativeRotation(FRotator(0.0f, 12.0f, -8.0f));
    WeaponMesh->SetRelativeScale3D(FVector(0.08f, 0.08f, 1.35f));
    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
    ShieldMesh->SetupAttachment(GetCapsuleComponent());
    ShieldMesh->SetRelativeLocation(FVector(0.0f, -28.0f, -20.0f));
    ShieldMesh->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
    ShieldMesh->SetRelativeScale3D(FVector(0.5f, 0.1f, 0.68f));
    ShieldMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    CrownMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrownMesh"));
    CrownMesh->SetupAttachment(GetCapsuleComponent());
    CrownMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 36.0f));
    CrownMesh->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.1f));
    CrownMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CapsuleMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMesh(TEXT("/Engine/BasicShapes/Cone.Cone"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CapsuleMesh.Succeeded())
    {
        BodyMesh->SetStaticMesh(CapsuleMesh.Object);
        WeaponMesh->SetStaticMesh(CapsuleMesh.Object);
        CrownMesh->SetStaticMesh(CapsuleMesh.Object);
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
        ShieldMesh->SetStaticMesh(CubeMesh.Object);
    }

    ConfigureProxyVisuals();

    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = false;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = ANazareneEnemyAIController::StaticClass();

    GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -92.0f));
    GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
    GetMesh()->SetAnimInstanceClass(UNazareneEnemyAnimInstance::StaticClass());

    if (!ProductionSkeletalMesh.ToSoftObjectPath().IsValid())
    {
        const FString ResolvedEnemyMesh = NazareneAssetResolver::ResolveObjectPath(
            TEXT("EnemySkeletalMesh"),
            TEXT("/Game/Art/Characters/Enemies/SK_BiblicalLegionary.SK_BiblicalLegionary"),
            {
                TEXT("/Game/Characters/MedievalOrientalArmour/SK_MOH_Soldier.SK_MOH_Soldier"),
                TEXT("/Game/Characters/MedievalOrientalArmour/Meshes/SK_MOH_Soldier.SK_MOH_Soldier"),
                TEXT("/Game/MedievalOrientalArmour/SK_MOH_Soldier.SK_MOH_Soldier")
            });
        ProductionSkeletalMesh = TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(ResolvedEnemyMesh));
    }
    if (!ProductionAnimBlueprint.ToSoftObjectPath().IsValid())
    {
        // Keep the default C++ anim instance class unless an explicit override is configured.
        const FString ResolvedEnemyAnimBP = NazareneAssetResolver::ResolveObjectPath(
            TEXT("EnemyAnimBlueprint"),
            TEXT(""),
            {});
        if (!ResolvedEnemyAnimBP.IsEmpty())
        {
            ProductionAnimBlueprint = TSoftClassPtr<UAnimInstance>(FSoftObjectPath(ResolvedEnemyAnimBP));
        }
    }

    if (AttackSound == nullptr)
    {
        AttackSound = LoadObject<USoundBase>(nullptr, TEXT("/Game/Audio/SFX/S_AttackWhoosh.S_AttackWhoosh"));
    }
    if (HitReactSound == nullptr)
    {
        HitReactSound = LoadObject<USoundBase>(nullptr, TEXT("/Game/Audio/SFX/S_Impact.S_Impact"));
    }
    if (RedeemedSound == nullptr)
    {
        RedeemedSound = LoadObject<USoundBase>(nullptr, TEXT("/Game/Audio/SFX/S_MiracleShimmer.S_MiracleShimmer"));
    }
}

void ANazareneEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    TSoftObjectPtr<USkeletalMesh> ResolvedProductionMesh = ProductionSkeletalMesh;
    if (const TCHAR* MeshKey = EnemyMeshOverrideKey(Archetype))
    {
        const FString BaseMeshPath = ProductionSkeletalMesh.ToSoftObjectPath().IsValid()
            ? ProductionSkeletalMesh.ToSoftObjectPath().ToString()
            : TEXT("/Game/Art/Characters/Enemies/SK_BiblicalLegionary.SK_BiblicalLegionary");
        const FString ResolvedMeshPath = NazareneAssetResolver::ResolveObjectPath(
            MeshKey,
            BaseMeshPath,
            {
                TEXT("/Game/Art/Characters/Enemies/SK_BiblicalLegionary.SK_BiblicalLegionary"),
                TEXT("/Game/Characters/MedievalOrientalArmour/SK_MOH_Soldier.SK_MOH_Soldier"),
                TEXT("/Game/Characters/MedievalOrientalArmour/Meshes/SK_MOH_Soldier.SK_MOH_Soldier")
            });
        ResolvedProductionMesh = TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(ResolvedMeshPath));
    }

    TSoftClassPtr<UAnimInstance> ResolvedProductionAnim = ProductionAnimBlueprint;
    if (const TCHAR* AnimKey = EnemyAnimOverrideKey(Archetype))
    {
        const FString BaseAnimPath = ProductionAnimBlueprint.ToSoftObjectPath().IsValid()
            ? ProductionAnimBlueprint.ToSoftObjectPath().ToString()
            : TEXT("");
        const FString ResolvedAnimPath = NazareneAssetResolver::ResolveObjectPath(
            AnimKey,
            BaseAnimPath,
            {});
        if (!ResolvedAnimPath.IsEmpty())
        {
            ResolvedProductionAnim = TSoftClassPtr<UAnimInstance>(FSoftObjectPath(ResolvedAnimPath));
        }
    }

    bool bAppliedCharacterMesh = false;
    if (ResolvedProductionMesh.ToSoftObjectPath().IsValid())
    {
        if (USkeletalMesh* MeshAsset = ResolvedProductionMesh.LoadSynchronous())
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

    if (ResolvedProductionAnim.ToSoftObjectPath().IsValid())
    {
        if (UClass* AnimClass = ResolvedProductionAnim.LoadSynchronous())
        {
            GetMesh()->SetAnimInstanceClass(AnimClass);
        }
    }

    SetProxyVisualsHidden(bAppliedCharacterMesh);

    if (SpawnId.IsNone())
    {
        SpawnId = FName(*FString::Printf(TEXT("%s_%d"), *EnemyName.Replace(TEXT(" "), TEXT("_")).ToLower(), GetUniqueID()));
    }

    ConfigureFromArchetype();

    SpawnLocation = GetActorLocation();
    SpawnRotation = GetActorRotation();

    CurrentHealth = MaxHealth;
    CurrentPoise = MaxPoise;
    CurrentState = ENazareneEnemyState::Idle;

    SyncTargetFromAIController();

    if (ANazareneEnemyAIController* AIController = Cast<ANazareneEnemyAIController>(GetController()))
    {
        AIController->SetBehaviorTreeAsset(BehaviorTreeAsset);
        AIController->SetFallbackTarget(TargetPlayer.Get());
    }
}

void ANazareneEnemyCharacter::ConfigureProxyVisuals()
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

    ApplyTint(BodyMesh, FLinearColor(0.48f, 0.42f, 0.36f));
    ApplyTint(HeadMesh, FLinearColor(0.62f, 0.50f, 0.40f));
    ApplyTint(MantleMesh, FLinearColor(0.38f, 0.18f, 0.18f));
    ApplyTint(RobeMesh, FLinearColor(0.35f, 0.26f, 0.19f));
    ApplyTint(WeaponMesh, FLinearColor(0.40f, 0.30f, 0.18f));
    ApplyTint(ShieldMesh, FLinearColor(0.31f, 0.23f, 0.16f));
    ApplyTint(CrownMesh, FLinearColor(0.70f, 0.60f, 0.26f));
}

void ANazareneEnemyCharacter::SetProxyVisualsHidden(bool bHideProxy)
{
    if (BodyMesh != nullptr)
    {
        BodyMesh->SetHiddenInGame(bHideProxy);
    }
    if (HeadMesh != nullptr)
    {
        HeadMesh->SetHiddenInGame(bHideProxy);
    }
    if (MantleMesh != nullptr)
    {
        MantleMesh->SetHiddenInGame(bHideProxy);
    }
    if (RobeMesh != nullptr)
    {
        RobeMesh->SetHiddenInGame(bHideProxy);
    }
    if (WeaponMesh != nullptr)
    {
        WeaponMesh->SetHiddenInGame(bHideProxy);
    }
    if (ShieldMesh != nullptr)
    {
        ShieldMesh->SetHiddenInGame(bHideProxy);
    }
    if (CrownMesh != nullptr)
    {
        CrownMesh->SetHiddenInGame(bHideProxy);
    }
}

void ANazareneEnemyCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (CurrentState == ENazareneEnemyState::Redeemed)
    {
        return;
    }

    SyncTargetFromAIController();

    if (!TargetPlayer.IsValid())
    {
        return;
    }

    UpdateBossPhase();
    ApplyPhaseSpecificBehavior(DeltaSeconds);

    ShotCooldown = FMath::Max(0.0f, ShotCooldown - DeltaSeconds);
    DashCooldown = FMath::Max(0.0f, DashCooldown - DeltaSeconds);
    CurrentPoise = FMath::Min(MaxPoise, CurrentPoise + PoiseRegen * DeltaSeconds);

    const FVector ToPlayer = TargetPlayer->GetActorLocation() - GetActorLocation();
    const float DistanceToPlayer = ToPlayer.Size2D();

    switch (CurrentState)
    {
    case ENazareneEnemyState::Idle:
        SlowToStop(DeltaSeconds);
        if (DistanceToPlayer <= DetectionRange)
        {
            CurrentState = ENazareneEnemyState::Chase;
        }
        break;

    case ENazareneEnemyState::Chase:
        FaceTarget(DeltaSeconds);
        if (DistanceToPlayer > DetectionRange * 1.35f)
        {
            CurrentState = ENazareneEnemyState::Idle;
        }
        else
        {
            ProcessChaseBehavior(DistanceToPlayer, DeltaSeconds);
        }
        break;

    case ENazareneEnemyState::Windup:
        ProcessWindup(DeltaSeconds, false);
        break;

    case ENazareneEnemyState::Casting:
        ProcessWindup(DeltaSeconds, true);
        break;

    case ENazareneEnemyState::Blocking:
    case ENazareneEnemyState::Recover:
    case ENazareneEnemyState::Staggered:
    case ENazareneEnemyState::Parried:
        StateTimer = FMath::Max(0.0f, StateTimer - DeltaSeconds);
        SlowToStop(DeltaSeconds);
        if (StateTimer <= 0.0f)
        {
            if (CurrentState == ENazareneEnemyState::Parried)
            {
                CurrentState = ENazareneEnemyState::Staggered;
                StateTimer = StaggerDuration * 0.55f;
            }
            else
            {
                CurrentState = ENazareneEnemyState::Chase;
            }
        }
        break;

    case ENazareneEnemyState::Retreat:
        MoveAwayFromTarget(DeltaSeconds);
        if (DistanceToPlayer >= MinimumRange + 70.0f)
        {
            CurrentState = ENazareneEnemyState::Chase;
        }
        break;

    case ENazareneEnemyState::Strafe:
        StrafeAroundTarget(DeltaSeconds);
        StateTimer = FMath::Max(0.0f, StateTimer - DeltaSeconds);
        if (StateTimer <= 0.0f)
        {
            CurrentState = ENazareneEnemyState::Chase;
        }
        break;

    default:
        break;
    }
}

void ANazareneEnemyCharacter::SyncTargetFromAIController()
{
    if (ANazareneEnemyAIController* AIController = Cast<ANazareneEnemyAIController>(GetController()))
    {
        if (AActor* AITrackedTarget = AIController->GetCurrentTargetActor())
        {
            if (ANazarenePlayerCharacter* PlayerTarget = Cast<ANazarenePlayerCharacter>(AITrackedTarget))
            {
                TargetPlayer = PlayerTarget;
                return;
            }
        }

        if (!TargetPlayer.IsValid())
        {
            APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
            TargetPlayer = Cast<ANazarenePlayerCharacter>(Pawn);
        }

        AIController->SetFallbackTarget(TargetPlayer.Get());
        return;
    }

    if (!TargetPlayer.IsValid())
    {
        APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
        TargetPlayer = Cast<ANazarenePlayerCharacter>(Pawn);
    }
}

void ANazareneEnemyCharacter::ConfigureFromArchetype()
{
    switch (Archetype)
    {
    case ENazareneEnemyArchetype::MeleeShield:
        if (EnemyName == TEXT("Roman Patrol"))
        {
            EnemyName = TEXT("Roman Shieldbearer");
        }
        MaxHealth = 110.0f;
        MaxPoise = 105.0f;
        MoveSpeed = 310.0f;
        AttackDamage = 16.0f;
        PostureDamage = 25.0f;
        AttackRange = 200.0f;
        AttackWindup = 0.46f;
        AttackRecovery = 1.15f;
        bCanParryAttacks = true;
        ShieldBlockChance = 0.62f;
        break;

    case ENazareneEnemyArchetype::Spear:
        if (EnemyName == TEXT("Roman Patrol"))
        {
            EnemyName = TEXT("Roman Spearman");
        }
        MaxHealth = 86.0f;
        MaxPoise = 72.0f;
        MoveSpeed = 400.0f;
        AttackDamage = 20.0f;
        PostureDamage = 24.0f;
        AttackRange = 340.0f;
        AttackWindup = 0.52f;
        AttackRecovery = 1.05f;
        bCanParryAttacks = true;
        ShieldBlockChance = 0.0f;
        break;

    case ENazareneEnemyArchetype::Ranged:
        if (EnemyName == TEXT("Roman Patrol"))
        {
            EnemyName = TEXT("Roman Slinger");
        }
        MaxHealth = 70.0f;
        MaxPoise = 55.0f;
        MoveSpeed = 360.0f;
        AttackDamage = 15.0f;
        PostureDamage = 18.0f;
        AttackRange = 1300.0f;
        MinimumRange = 480.0f;
        AttackWindup = 0.62f;
        AttackRecovery = 0.75f;
        bCanParryAttacks = false;
        ShieldBlockChance = 0.0f;
        ProjectileSpeed = 2000.0f;
        break;

    case ENazareneEnemyArchetype::Demon:
        if (EnemyName == TEXT("Roman Patrol"))
        {
            EnemyName = TEXT("Unclean Spirit");
        }
        MaxHealth = 94.0f;
        MaxPoise = 65.0f;
        MoveSpeed = 480.0f;
        AttackDamage = 22.0f;
        PostureDamage = 24.0f;
        AttackRange = 240.0f;
        AttackWindup = 0.34f;
        AttackRecovery = 0.82f;
        bCanParryAttacks = true;
        ShieldBlockChance = 0.0f;
        break;

    case ENazareneEnemyArchetype::Boss:
        if (EnemyName == TEXT("Roman Patrol"))
        {
            EnemyName = TEXT("Legion Sovereign");
        }
        MaxHealth = 420.0f;
        MaxPoise = 210.0f;
        MoveSpeed = 390.0f;
        AttackDamage = 29.0f;
        PostureDamage = 36.0f;
        DetectionRange = 2800.0f;
        AttackRange = 360.0f;
        MinimumRange = 700.0f;
        AttackWindup = 0.58f;
        AttackRecovery = 0.95f;
        ParryWindowStartRatio = 0.42f;
        ParryWindowEndRatio = 0.66f;
        ParryVulnerabilityDuration = 1.15f;
        StaggerDuration = 0.92f;
        ProjectileSpeed = 2200.0f;
        bCanParryAttacks = true;
        ShieldBlockChance = 0.0f;
        break;
    }

    CurrentHealth = MaxHealth;
    CurrentPoise = MaxPoise;
    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
    ApplyProxyArchetypeVisualStyle();
}

void ANazareneEnemyCharacter::ApplyProxyArchetypeVisualStyle()
{
    auto Tint = [](UStaticMeshComponent* Component, const FLinearColor& Color)
    {
        if (Component == nullptr)
        {
            return;
        }
        Component->SetVectorParameterValueOnMaterials(TEXT("Color"), FVector(Color.R, Color.G, Color.B));
    };

    if (ShieldMesh != nullptr)
    {
        ShieldMesh->SetVisibility(false);
    }
    if (CrownMesh != nullptr)
    {
        CrownMesh->SetVisibility(false);
    }

    if (WeaponMesh != nullptr)
    {
        WeaponMesh->SetRelativeLocation(FVector(26.0f, 18.0f, -40.0f));
        WeaponMesh->SetRelativeRotation(FRotator(0.0f, 12.0f, -8.0f));
        WeaponMesh->SetRelativeScale3D(FVector(0.08f, 0.08f, 1.35f));
    }
    if (ShieldMesh != nullptr)
    {
        ShieldMesh->SetRelativeLocation(FVector(0.0f, -28.0f, -20.0f));
        ShieldMesh->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
        ShieldMesh->SetRelativeScale3D(FVector(0.5f, 0.1f, 0.68f));
    }
    if (RobeMesh != nullptr)
    {
        RobeMesh->SetRelativeScale3D(FVector(0.75f, 0.75f, 1.58f));
    }

    const FString RomanMaterialPath = NazareneAssetResolver::ResolveObjectPath(
        TEXT("EnemyMaterialRoman"),
        TEXT("/Game/Art/Materials/MI_Character_RomanArmor.MI_Character_RomanArmor"),
        {});
    const FString DemonMaterialPath = NazareneAssetResolver::ResolveObjectPath(
        TEXT("EnemyMaterialDemon"),
        TEXT("/Game/Art/Materials/MI_Character_Demon.MI_Character_Demon"),
        {});
    const FString BossMaterialPath = NazareneAssetResolver::ResolveObjectPath(
        TEXT("EnemyMaterialBoss"),
        TEXT("/Game/Art/Materials/MI_Character_Boss.MI_Character_Boss"),
        {});

    UMaterialInterface* SkeletalMaterial = nullptr;

    switch (Archetype)
    {
    case ENazareneEnemyArchetype::MeleeShield:
        if (ShieldMesh != nullptr)
        {
            ShieldMesh->SetVisibility(true);
        }
        Tint(MantleMesh, FLinearColor(0.47f, 0.18f, 0.16f));
        Tint(RobeMesh, FLinearColor(0.43f, 0.34f, 0.26f));
        Tint(WeaponMesh, FLinearColor(0.34f, 0.30f, 0.24f));
        Tint(ShieldMesh, FLinearColor(0.30f, 0.22f, 0.15f));
        SkeletalMaterial = LoadObject<UMaterialInterface>(nullptr, *RomanMaterialPath);
        break;

    case ENazareneEnemyArchetype::Spear:
        if (WeaponMesh != nullptr)
        {
            WeaponMesh->SetRelativeLocation(FVector(32.0f, 8.0f, -32.0f));
            WeaponMesh->SetRelativeRotation(FRotator(0.0f, -4.0f, -2.0f));
            WeaponMesh->SetRelativeScale3D(FVector(0.06f, 0.06f, 2.3f));
        }
        Tint(MantleMesh, FLinearColor(0.53f, 0.24f, 0.16f));
        Tint(RobeMesh, FLinearColor(0.49f, 0.40f, 0.30f));
        Tint(WeaponMesh, FLinearColor(0.37f, 0.30f, 0.20f));
        SkeletalMaterial = LoadObject<UMaterialInterface>(nullptr, *RomanMaterialPath);
        break;

    case ENazareneEnemyArchetype::Ranged:
        if (WeaponMesh != nullptr)
        {
            WeaponMesh->SetRelativeLocation(FVector(30.0f, 14.0f, -34.0f));
            WeaponMesh->SetRelativeScale3D(FVector(0.23f, 0.23f, 0.35f));
        }
        Tint(MantleMesh, FLinearColor(0.30f, 0.24f, 0.20f));
        Tint(RobeMesh, FLinearColor(0.34f, 0.31f, 0.26f));
        Tint(WeaponMesh, FLinearColor(0.45f, 0.36f, 0.24f));
        SkeletalMaterial = LoadObject<UMaterialInterface>(nullptr, *RomanMaterialPath);
        break;

    case ENazareneEnemyArchetype::Demon:
        if (WeaponMesh != nullptr)
        {
            WeaponMesh->SetRelativeLocation(FVector(20.0f, 10.0f, -32.0f));
            WeaponMesh->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
        }
        Tint(HeadMesh, FLinearColor(0.21f, 0.12f, 0.10f));
        Tint(MantleMesh, FLinearColor(0.18f, 0.08f, 0.08f));
        Tint(RobeMesh, FLinearColor(0.12f, 0.07f, 0.08f));
        Tint(WeaponMesh, FLinearColor(0.80f, 0.16f, 0.14f));
        SkeletalMaterial = LoadObject<UMaterialInterface>(nullptr, *DemonMaterialPath);
        break;

    case ENazareneEnemyArchetype::Boss:
        if (ShieldMesh != nullptr)
        {
            ShieldMesh->SetVisibility(true);
            ShieldMesh->SetRelativeLocation(FVector(4.0f, -32.0f, -6.0f));
            ShieldMesh->SetRelativeScale3D(FVector(0.74f, 0.14f, 1.06f));
        }
        if (WeaponMesh != nullptr)
        {
            WeaponMesh->SetRelativeLocation(FVector(40.0f, 14.0f, -18.0f));
            WeaponMesh->SetRelativeScale3D(FVector(0.09f, 0.09f, 2.75f));
        }
        if (RobeMesh != nullptr)
        {
            RobeMesh->SetRelativeScale3D(FVector(0.9f, 0.9f, 2.0f));
        }
        if (CrownMesh != nullptr)
        {
            CrownMesh->SetVisibility(true);
        }
        Tint(MantleMesh, FLinearColor(0.47f, 0.18f, 0.15f));
        Tint(RobeMesh, FLinearColor(0.21f, 0.13f, 0.12f));
        Tint(WeaponMesh, FLinearColor(0.56f, 0.42f, 0.18f));
        Tint(ShieldMesh, FLinearColor(0.40f, 0.29f, 0.14f));
        Tint(CrownMesh, FLinearColor(0.78f, 0.62f, 0.24f));
        SkeletalMaterial = LoadObject<UMaterialInterface>(nullptr, *BossMaterialPath);
        break;
    }

    if (GetMesh() != nullptr && GetMesh()->GetSkeletalMeshAsset() != nullptr && SkeletalMaterial != nullptr)
    {
        GetMesh()->SetMaterial(0, SkeletalMaterial);
    }
}

bool ANazareneEnemyCharacter::CanBeParried() const
{
    if (!bCanParryAttacks || CurrentState != ENazareneEnemyState::Windup || WindupDuration <= KINDA_SMALL_NUMBER)
    {
        return false;
    }

    const float Ratio = WindupElapsed / WindupDuration;
    return Ratio >= EffectiveParryStartRatio() && Ratio <= EffectiveParryEndRatio();
}

bool ANazareneEnemyCharacter::IsParried() const
{
    return CurrentState == ENazareneEnemyState::Parried;
}

bool ANazareneEnemyCharacter::IsRedeemed() const
{
    return CurrentState == ENazareneEnemyState::Redeemed;
}

ENazareneEnemyState ANazareneEnemyCharacter::GetState() const
{
    return CurrentState;
}

float ANazareneEnemyCharacter::GetStateTimerRemaining() const
{
    return StateTimer;
}

void ANazareneEnemyCharacter::OnParried(ANazarenePlayerCharacter* ByPlayer)
{
    if (CurrentState == ENazareneEnemyState::Redeemed)
    {
        return;
    }

    CurrentState = ENazareneEnemyState::Parried;
    StateTimer = ParryVulnerabilityDuration;
    if (Archetype == ENazareneEnemyArchetype::Boss)
    {
        StateTimer *= 0.72f;
    }
    CurrentPoise = MaxPoise * 0.5f;

    if (ByPlayer != nullptr)
    {
        ByPlayer->AddFaith(3.0f);
    }
}

void ANazareneEnemyCharacter::ReceiveCombatHit(float Damage, float PoiseDamage, ANazarenePlayerCharacter* Source)
{
    if (CurrentState == ENazareneEnemyState::Redeemed)
    {
        return;
    }

    if (TryShieldBlock(Source, PoiseDamage))
    {
        return;
    }

    CurrentHealth -= Damage;
    CurrentPoise -= PoiseDamage;
    TriggerPresentation(HitReactSound, HitReactVFX, GetActorLocation(), 0.8f);

    if (CurrentPoise <= 0.0f)
    {
        CurrentState = ENazareneEnemyState::Staggered;
        StateTimer = StaggerDuration;
        if (Archetype == ENazareneEnemyArchetype::Boss)
        {
            StateTimer *= 0.68f;
        }
        CurrentPoise = MaxPoise;
        if (Source != nullptr)
        {
            Source->DisplayDamageNumber(GetActorLocation() + FVector(0.0f, 0.0f, 150.0f), PoiseDamage, ENazareneDamageNumberType::PoiseBreak);
        }
    }

    if (CurrentHealth <= 0.0f)
    {
        BecomeRedeemed(Source, true);
    }
}

void ANazareneEnemyCharacter::ReceiveRiposte(float Damage, ANazarenePlayerCharacter* Source)
{
    if (CurrentState != ENazareneEnemyState::Parried && CurrentState != ENazareneEnemyState::Staggered)
    {
        return;
    }

    CurrentHealth -= Damage;
    if (CurrentHealth <= 0.0f)
    {
        BecomeRedeemed(Source, true);
        return;
    }

    CurrentState = ENazareneEnemyState::Staggered;
    StateTimer = StaggerDuration;
}

void ANazareneEnemyCharacter::ApplyKnockback(const FVector& Direction, float Force)
{
    if (CurrentState == ENazareneEnemyState::Redeemed || Direction.IsNearlyZero())
    {
        return;
    }
    LaunchCharacter(Direction.GetSafeNormal2D() * Force, true, false);
}

void ANazareneEnemyCharacter::ResetToSpawn()
{
    SetActorLocation(SpawnLocation);
    SetActorRotation(SpawnRotation);
    CurrentHealth = MaxHealth;
    CurrentPoise = MaxPoise;
    CurrentState = ENazareneEnemyState::Idle;
    StateTimer = 0.0f;
    BossPhase = 1;
    WindupElapsed = 0.0f;
    WindupDuration = 0.0f;
    bAttackResolved = false;
    ShotCooldown = 0.0f;
    DashCooldown = 0.0f;
    bPhase2WaveSpawned = false;
    bPhase3WaveSpawned = false;
    DoubleStrikeCooldown = 0.0f;
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

FNazareneEnemySnapshot ANazareneEnemyCharacter::BuildSnapshot() const
{
    FNazareneEnemySnapshot Snapshot;
    Snapshot.SpawnId = SpawnId;
    Snapshot.EnemyName = EnemyName;
    Snapshot.Archetype = Archetype;
    Snapshot.Position = GetActorLocation();
    Snapshot.Health = CurrentHealth;
    Snapshot.Poise = CurrentPoise;
    Snapshot.bRedeemed = (CurrentState == ENazareneEnemyState::Redeemed);
    Snapshot.BossPhase = BossPhase;
    return Snapshot;
}

void ANazareneEnemyCharacter::ApplySnapshot(const FNazareneEnemySnapshot& Snapshot)
{
    if (Snapshot.bRedeemed)
    {
        BecomeRedeemed(nullptr, false);
        return;
    }

    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);

    SetActorLocation(Snapshot.Position);
    CurrentHealth = FMath::Clamp(Snapshot.Health, 0.0f, MaxHealth);
    CurrentPoise = FMath::Clamp(Snapshot.Poise, 0.0f, MaxPoise);
    BossPhase = FMath::Clamp(Snapshot.BossPhase, 1, 3);

    CurrentState = ENazareneEnemyState::Idle;
    StateTimer = 0.0f;
    WindupElapsed = 0.0f;
    WindupDuration = 0.0f;
    bAttackResolved = false;
    ShotCooldown = 0.0f;
    DashCooldown = 0.0f;
    bPhase2WaveSpawned = false;
    bPhase3WaveSpawned = false;
    DoubleStrikeCooldown = 0.0f;
}

void ANazareneEnemyCharacter::UpdateBossPhase()
{
    if (Archetype != ENazareneEnemyArchetype::Boss)
    {
        return;
    }

    const float HealthRatio = MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 1.0f;
    int32 NextPhase = 1;
    if (HealthRatio <= 0.33f)
    {
        NextPhase = 3;
    }
    else if (HealthRatio <= 0.66f)
    {
        NextPhase = 2;
    }

    if (NextPhase != BossPhase)
    {
        BossPhase = NextPhase;
        OnPhaseChanged.Broadcast(this, BossPhase);
        CheckReinforcementTrigger();
    }
}

void ANazareneEnemyCharacter::CheckReinforcementTrigger()
{
    if (Archetype != ENazareneEnemyArchetype::Boss)
    {
        return;
    }

    if (BossPhase >= 2 && !bPhase2WaveSpawned)
    {
        bPhase2WaveSpawned = true;
    }
    if (BossPhase >= 3 && !bPhase3WaveSpawned)
    {
        bPhase3WaveSpawned = true;
    }
}

void ANazareneEnemyCharacter::TriggerArenaHazard()
{
    if (Archetype != ENazareneEnemyArchetype::Boss)
    {
        return;
    }
    OnArenaHazardTriggered.Broadcast(this, GetActorLocation());
}

void ANazareneEnemyCharacter::ApplyPhaseSpecificBehavior(float DeltaSeconds)
{
    if (Archetype != ENazareneEnemyArchetype::Boss)
    {
        return;
    }

    if (DoubleStrikeCooldown > 0.0f)
    {
        DoubleStrikeCooldown -= DeltaSeconds;
    }
}

void ANazareneEnemyCharacter::ProcessChaseBehavior(float DistanceToPlayer, float DeltaSeconds)
{
    switch (Archetype)
    {
    case ENazareneEnemyArchetype::Ranged:
        if (DistanceToPlayer < MinimumRange)
        {
            CurrentState = ENazareneEnemyState::Retreat;
        }
        else if (DistanceToPlayer <= AttackRange && ShotCooldown <= 0.0f)
        {
            BeginCastAttack();
        }
        else if (DistanceToPlayer > AttackRange * 0.92f)
        {
            MoveTowardTarget(DeltaSeconds, EffectiveMoveSpeed());
        }
        else
        {
            CurrentState = ENazareneEnemyState::Strafe;
            StateTimer = FMath::FRandRange(0.65f, 1.2f);
            StrafeDirectionSign = (FMath::FRand() > 0.5f) ? 1 : -1;
        }
        break;

    case ENazareneEnemyArchetype::Demon:
        if (DistanceToPlayer <= AttackRange * 1.25f)
        {
            if (DashCooldown <= 0.0f && FMath::FRand() < 0.32f)
            {
                FVector Dir = TargetPlayer->GetActorLocation() - GetActorLocation();
                Dir.Z = 0.0f;
                LaunchCharacter(Dir.GetSafeNormal() * EffectiveMoveSpeed() * 1.9f, true, false);
                DashCooldown = 2.0f;
            }
            BeginMeleeAttack();
        }
        else
        {
            MoveTowardTarget(DeltaSeconds, EffectiveMoveSpeed() * 1.08f);
        }
        break;

    case ENazareneEnemyArchetype::Boss:
        if (BossPhase >= 2 && DistanceToPlayer > AttackRange + 250.0f && ShotCooldown <= 0.0f && FMath::FRand() < Phase2CastFrequency)
        {
            BeginCastAttack();
        }
        else if (DistanceToPlayer <= AttackRange + 40.0f)
        {
            BeginMeleeAttack();
        }
        else
        {
            MoveTowardTarget(DeltaSeconds, EffectiveMoveSpeed());
        }
        if (BossPhase >= 3 && DashCooldown <= 0.0f && TargetPlayer.IsValid())
        {
            const float DistToPlayer = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());
            if (DistToPlayer > AttackRange * 1.5f && DistToPlayer < DetectionRange && FMath::FRand() < Phase3DashFrequency * DeltaSeconds)
            {
                FVector DashDir = TargetPlayer->GetActorLocation() - GetActorLocation();
                DashDir.Z = 0.0f;
                DashDir = DashDir.GetSafeNormal();
                LaunchCharacter(DashDir * EffectiveMoveSpeed() * 2.2f, true, false);
                DashCooldown = 2.8f;
                TriggerArenaHazard();
            }
        }
        break;

    default:
        if (DistanceToPlayer <= AttackRange)
        {
            BeginMeleeAttack();
        }
        else
        {
            MoveTowardTarget(DeltaSeconds, EffectiveMoveSpeed());
        }
        break;
    }
}

void ANazareneEnemyCharacter::BeginMeleeAttack()
{
    if (CurrentState != ENazareneEnemyState::Chase)
    {
        return;
    }

    CurrentState = ENazareneEnemyState::Windup;
    WindupDuration = EffectiveWindup();
    WindupElapsed = 0.0f;
    bAttackResolved = false;
    StateTimer = WindupDuration;
    TriggerPresentation(AttackSound, AttackVFX, GetActorLocation() + GetActorForwardVector() * 100.0f, 0.75f);
}

void ANazareneEnemyCharacter::BeginCastAttack()
{
    if (CurrentState != ENazareneEnemyState::Chase)
    {
        return;
    }

    CurrentState = ENazareneEnemyState::Casting;
    WindupDuration = FMath::Max(0.22f, EffectiveWindup() + 0.08f);
    WindupElapsed = 0.0f;
    bAttackResolved = false;
    StateTimer = WindupDuration;
    TriggerPresentation(AttackSound, AttackVFX, GetActorLocation() + GetActorForwardVector() * 130.0f, 0.72f);
}

void ANazareneEnemyCharacter::ProcessWindup(float DeltaSeconds, bool bCasting)
{
    FaceTarget(DeltaSeconds);
    StateTimer = FMath::Max(0.0f, StateTimer - DeltaSeconds);
    WindupElapsed += DeltaSeconds;
    SlowToStop(DeltaSeconds);

    if (!bAttackResolved && WindupElapsed >= WindupDuration * StrikeTimingRatio)
    {
        bAttackResolved = true;
        if (bCasting)
        {
            ResolveCastAttack();
        }
        else
        {
            ResolveMeleeAttack();
        }
    }

    if (StateTimer <= 0.0f)
    {
        CurrentState = ENazareneEnemyState::Recover;
        StateTimer = EffectiveRecovery();
    }
}

void ANazareneEnemyCharacter::ResolveMeleeAttack()
{
    if (!TargetPlayer.IsValid())
    {
        return;
    }

    float BonusRange = 80.0f;
    if (Archetype == ENazareneEnemyArchetype::Spear)
    {
        BonusRange = 110.0f;
    }
    if (Archetype == ENazareneEnemyArchetype::Boss && BossPhase >= 2)
    {
        BonusRange = 120.0f;
    }

    const float Dist = FVector::Dist2D(GetActorLocation(), TargetPlayer->GetActorLocation());
    if (Dist <= AttackRange + BonusRange)
    {
        TargetPlayer->ReceiveEnemyAttack(this, EffectiveAttackDamage(), EffectivePostureDamage());
    }
}

void ANazareneEnemyCharacter::ResolveCastAttack()
{
    if (!TargetPlayer.IsValid())
    {
        return;
    }

    const float Dist = FVector::Dist2D(GetActorLocation(), TargetPlayer->GetActorLocation());
    if (Dist <= AttackRange * 1.65f)
    {
        TargetPlayer->ReceiveEnemyAttack(this, EffectiveAttackDamage() * 0.88f, EffectivePostureDamage() * 0.74f);
    }

    ShotCooldown = 1.65f / PhaseSpeedScale();
}

void ANazareneEnemyCharacter::FaceTarget(float DeltaSeconds)
{
    if (!TargetPlayer.IsValid())
    {
        return;
    }

    FVector Dir = TargetPlayer->GetActorLocation() - GetActorLocation();
    Dir.Z = 0.0f;
    if (Dir.IsNearlyZero())
    {
        return;
    }

    const FRotator DesiredRot = Dir.Rotation();
    const FRotator NewRot = FMath::RInterpTo(GetActorRotation(), DesiredRot, DeltaSeconds, 11.0f);
    SetActorRotation(FRotator(0.0f, NewRot.Yaw, 0.0f));
}

void ANazareneEnemyCharacter::MoveTowardTarget(float DeltaSeconds, float Speed)
{
    if (!TargetPlayer.IsValid())
    {
        return;
    }

    FVector Dir = TargetPlayer->GetActorLocation() - GetActorLocation();
    Dir.Z = 0.0f;
    if (Dir.IsNearlyZero())
    {
        return;
    }

    GetCharacterMovement()->MaxWalkSpeed = Speed;
    AddMovementInput(Dir.GetSafeNormal(), 1.0f);
}

void ANazareneEnemyCharacter::MoveAwayFromTarget(float DeltaSeconds)
{
    if (!TargetPlayer.IsValid())
    {
        return;
    }

    FVector Dir = GetActorLocation() - TargetPlayer->GetActorLocation();
    Dir.Z = 0.0f;
    if (Dir.IsNearlyZero())
    {
        return;
    }

    GetCharacterMovement()->MaxWalkSpeed = EffectiveMoveSpeed();
    AddMovementInput(Dir.GetSafeNormal(), 1.0f);
    FaceTarget(DeltaSeconds);
}

void ANazareneEnemyCharacter::StrafeAroundTarget(float DeltaSeconds)
{
    if (!TargetPlayer.IsValid())
    {
        return;
    }

    FVector Forward = TargetPlayer->GetActorLocation() - GetActorLocation();
    Forward.Z = 0.0f;
    if (Forward.IsNearlyZero())
    {
        return;
    }
    Forward = Forward.GetSafeNormal();
    FVector Right(-Forward.Y, Forward.X, 0.0f);
    FVector Strafe = (Right * float(StrafeDirectionSign) + Forward * 0.18f).GetSafeNormal();

    GetCharacterMovement()->MaxWalkSpeed = EffectiveMoveSpeed() * 0.9f;
    AddMovementInput(Strafe, 1.0f);
    FaceTarget(DeltaSeconds);
}

void ANazareneEnemyCharacter::SlowToStop(float DeltaSeconds)
{
    FVector Velocity = GetCharacterMovement()->Velocity;
    Velocity.X = FMath::FInterpTo(Velocity.X, 0.0f, DeltaSeconds, 6.0f);
    Velocity.Y = FMath::FInterpTo(Velocity.Y, 0.0f, DeltaSeconds, 6.0f);
    GetCharacterMovement()->Velocity = Velocity;
}

bool ANazareneEnemyCharacter::TryShieldBlock(ANazarenePlayerCharacter* Source, float PoiseDamage)
{
    if (Archetype != ENazareneEnemyArchetype::MeleeShield || Source == nullptr)
    {
        return false;
    }

    if (CurrentState != ENazareneEnemyState::Chase && CurrentState != ENazareneEnemyState::Idle && CurrentState != ENazareneEnemyState::Recover)
    {
        return false;
    }

    if (FMath::FRand() > ShieldBlockChance)
    {
        return false;
    }

    FVector AttackDir = Source->GetActorLocation() - GetActorLocation();
    AttackDir.Z = 0.0f;
    if (AttackDir.IsNearlyZero())
    {
        return false;
    }

    const FVector Facing = GetActorForwardVector().GetSafeNormal2D();
    const bool bFrontal = FVector::DotProduct(Facing, AttackDir.GetSafeNormal2D()) > 0.3f;
    if (!bFrontal)
    {
        return false;
    }

    CurrentPoise = FMath::Max(0.0f, CurrentPoise - PoiseDamage * 0.35f);
    CurrentState = ENazareneEnemyState::Blocking;
    StateTimer = 0.26f;
    Source->DisplayDamageNumber(GetActorLocation() + FVector(0.0f, 0.0f, 130.0f), 0.0f, ENazareneDamageNumberType::Blocked);
    return true;
}

void ANazareneEnemyCharacter::BecomeRedeemed(ANazarenePlayerCharacter* Source, bool bGrantReward)
{
    CurrentState = ENazareneEnemyState::Redeemed;
    CurrentHealth = 0.0f;
    GetCharacterMovement()->StopMovementImmediately();
    GetCharacterMovement()->DisableMovement();
    SetActorEnableCollision(false);
    SetActorHiddenInGame(true);
    TriggerPresentation(RedeemedSound, RedeemedVFX, GetActorLocation(), 0.95f);

    if (bGrantReward)
    {
        if (Source != nullptr)
        {
            Source->AddFaith(FaithReward);
        }
        OnRedeemed.Broadcast(this, FaithReward);
    }
}

float ANazareneEnemyCharacter::EffectiveMoveSpeed() const
{
    float Value = MoveSpeed;
    if (Archetype == ENazareneEnemyArchetype::Boss)
    {
        Value *= PhaseSpeedScale();
    }
    return Value;
}

float ANazareneEnemyCharacter::EffectiveAttackDamage() const
{
    float Value = AttackDamage;
    if (Archetype == ENazareneEnemyArchetype::Boss)
    {
        Value *= (1.0f + float(BossPhase - 1) * 0.2f);
    }
    if (Archetype == ENazareneEnemyArchetype::Demon)
    {
        Value *= 1.08f;
    }
    return Value;
}

float ANazareneEnemyCharacter::EffectivePostureDamage() const
{
    float Value = PostureDamage;
    if (Archetype == ENazareneEnemyArchetype::Boss)
    {
        Value *= (1.0f + float(BossPhase - 1) * 0.16f);
    }
    return Value;
}

float ANazareneEnemyCharacter::EffectiveWindup() const
{
    float Value = AttackWindup;
    if (Archetype == ENazareneEnemyArchetype::Boss)
    {
        Value *= (1.0f - float(BossPhase - 1) * 0.08f);
    }
    return FMath::Max(Value, 0.16f);
}

float ANazareneEnemyCharacter::EffectiveRecovery() const
{
    float Value = AttackRecovery;
    if (Archetype == ENazareneEnemyArchetype::Boss)
    {
        Value *= (1.0f - float(BossPhase - 1) * 0.06f);
    }
    return FMath::Max(Value, 0.24f);
}

float ANazareneEnemyCharacter::EffectiveParryStartRatio() const
{
    float Value = ParryWindowStartRatio;
    if (Archetype == ENazareneEnemyArchetype::Boss)
    {
        Value += float(BossPhase - 1) * 0.05f;
    }
    return FMath::Clamp(Value, 0.1f, 0.84f);
}

float ANazareneEnemyCharacter::EffectiveParryEndRatio() const
{
    float Value = ParryWindowEndRatio;
    if (Archetype == ENazareneEnemyArchetype::Boss)
    {
        Value -= float(BossPhase - 1) * 0.05f;
    }
    return FMath::Clamp(Value, EffectiveParryStartRatio() + 0.06f, 0.95f);
}

float ANazareneEnemyCharacter::PhaseSpeedScale() const
{
    if (Archetype != ENazareneEnemyArchetype::Boss)
    {
        return 1.0f;
    }
    return 1.0f + float(BossPhase - 1) * 0.12f;
}

void ANazareneEnemyCharacter::TriggerPresentation(USoundBase* Sound, UNiagaraSystem* Effect, const FVector& Location, float VolumeMultiplier) const
{
    if (Sound != nullptr)
    {
        UGameplayStatics::PlaySoundAtLocation(this, Sound, Location, FRotator::ZeroRotator, VolumeMultiplier);
    }

    if (Effect != nullptr && GetWorld() != nullptr)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), Effect, Location, GetActorRotation());
    }
}
