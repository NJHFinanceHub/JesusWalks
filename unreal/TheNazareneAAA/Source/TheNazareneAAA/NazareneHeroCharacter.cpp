#include "NazareneHeroCharacter.h"
#include "NazareneEnemyCharacter.h"

ANazareneHeroCharacter::ANazareneHeroCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ANazareneHeroCharacter::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
    CurrentFaith = FMath::Clamp(CurrentFaith, 0.0f, MaxFaith);
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
    OnFaithChanged.Broadcast(CurrentFaith, MaxFaith);
}

float ANazareneHeroCharacter::ReceiveDamage(const float DamageAmount)
{
    CurrentHealth = FMath::Clamp(CurrentHealth - FMath::Max(DamageAmount, 0.0f), 0.0f, MaxHealth);
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
    return CurrentHealth;
}

bool ANazareneHeroCharacter::IsDead() const
{
    return CurrentHealth <= KINDA_SMALL_NUMBER;
}

float ANazareneHeroCharacter::PerformMeleeAttack(AActor* Target)
{
    if (!Target)
    {
        return 0.0f;
    }

    if (ANazareneEnemyCharacter* Enemy = Cast<ANazareneEnemyCharacter>(Target))
    {
        return Enemy->ReceiveDamage(BaseMeleeDamage);
    }

    return 0.0f;
}

void ANazareneHeroCharacter::RestoreAtPrayer(const float HealthRestorePercent, const float FaithRestorePercent)
{
    CurrentHealth = FMath::Clamp(CurrentHealth + (MaxHealth * FMath::Max(HealthRestorePercent, 0.0f)), 0.0f, MaxHealth);
    CurrentFaith = FMath::Clamp(CurrentFaith + (MaxFaith * FMath::Max(FaithRestorePercent, 0.0f)), 0.0f, MaxFaith);
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
    OnFaithChanged.Broadcast(CurrentFaith, MaxFaith);
}

bool ANazareneHeroCharacter::CastMiracleById(const FName MiracleId, AActor* Target, float& OutDamageDone)
{
    OutDamageDone = 0.0f;

    const FNazareneMiracle* Miracle = KnownMiracles.FindByPredicate([MiracleId](const FNazareneMiracle& Candidate)
    {
        return Candidate.MiracleId == MiracleId;
    });

    if (!Miracle || !CanCastMiracle(*Miracle))
    {
        return false;
    }

    CurrentFaith = FMath::Clamp(CurrentFaith - Miracle->FaithCost, 0.0f, MaxFaith);
    OnFaithChanged.Broadcast(CurrentFaith, MaxFaith);

    if (ANazareneEnemyCharacter* Enemy = Cast<ANazareneEnemyCharacter>(Target))
    {
        OutDamageDone = Enemy->ReceiveDamage(Miracle->Power);
    }

    MarkMiracleCast(*Miracle);
    return true;
}

bool ANazareneHeroCharacter::CanCastMiracle(const FNazareneMiracle& Miracle) const
{
    if (CurrentFaith < Miracle.FaithCost)
    {
        return false;
    }

    if (const float* ReadyAt = MiracleReadyTimes.Find(Miracle.MiracleId))
    {
        return GetWorld() && GetWorld()->GetTimeSeconds() >= *ReadyAt;
    }

    return true;
}

void ANazareneHeroCharacter::MarkMiracleCast(const FNazareneMiracle& Miracle)
{
    if (!GetWorld())
    {
        return;
    }

    MiracleReadyTimes.FindOrAdd(Miracle.MiracleId) = GetWorld()->GetTimeSeconds() + Miracle.CooldownSeconds;
}
