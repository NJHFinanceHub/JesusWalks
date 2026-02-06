#include "NazareneEnemyCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NazarenePlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANazareneEnemyCharacter::ANazareneEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

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

    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = false;
}

void ANazareneEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

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
}

void ANazareneEnemyCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (CurrentState == ENazareneEnemyState::Redeemed)
    {
        return;
    }

    if (!TargetPlayer.IsValid())
    {
        APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
        TargetPlayer = Cast<ANazarenePlayerCharacter>(Pawn);
    }

    if (!TargetPlayer.IsValid())
    {
        return;
    }

    UpdateBossPhase();

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

void ANazareneEnemyCharacter::ReceiveHit(float Damage, float PoiseDamage, ANazarenePlayerCharacter* Source)
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

    if (CurrentPoise <= 0.0f)
    {
        CurrentState = ENazareneEnemyState::Staggered;
        StateTimer = StaggerDuration;
        if (Archetype == ENazareneEnemyArchetype::Boss)
        {
            StateTimer *= 0.68f;
        }
        CurrentPoise = MaxPoise;
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
        if (BossPhase >= 2 && DistanceToPlayer > AttackRange + 250.0f && ShotCooldown <= 0.0f && FMath::FRand() < 0.45f)
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
