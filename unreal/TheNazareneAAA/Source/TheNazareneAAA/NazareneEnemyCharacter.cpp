#include "NazareneEnemyCharacter.h"

ANazareneEnemyCharacter::ANazareneEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
    CurrentHealth = Archetype.MaxHealth;
}

void ANazareneEnemyCharacter::InitializeFromArchetype(const FNazareneEnemyArchetype& InArchetype)
{
    Archetype = InArchetype;
    CurrentHealth = Archetype.MaxHealth;
}

float ANazareneEnemyCharacter::ReceiveDamage(const float DamageAmount)
{
    CurrentHealth = FMath::Clamp(CurrentHealth - FMath::Max(0.0f, DamageAmount), 0.0f, Archetype.MaxHealth);
    return CurrentHealth;
}

bool ANazareneEnemyCharacter::IsDefeated() const
{
    return CurrentHealth <= KINDA_SMALL_NUMBER;
}
