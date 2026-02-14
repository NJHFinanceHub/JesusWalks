#include "GA_NazareneRadiance.h"

#include "EngineUtils.h"
#include "NazareneAttributeSet.h"
#include "NazareneEnemyCharacter.h"
#include "NazarenePlayerCharacter.h"
#include "GameplayTagsManager.h"

UGA_NazareneRadiance::UGA_NazareneRadiance()
{
    FaithCost = 30.0f;
    CooldownDuration = 12.0f;
    CooldownTag = FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Miracle.Radiance"));
}

void UGA_NazareneRadiance::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    ANazarenePlayerCharacter* Player = GetNazarenePlayerCharacter(ActorInfo);
    UNazareneAttributeSet* Attributes = GetNazareneAttributeSet(ActorInfo);

    if (Player == nullptr || Attributes == nullptr)
    {
        CancelAbility(Handle, ActorInfo, ActivationInfo, true);
        return;
    }

    // Check miracle unlock
    if (!Player->IsMiracleUnlocked(FName(TEXT("radiance"))))
    {
        Player->SetContextHint(TEXT("Radiance miracle not yet unlocked."));
        CancelAbility(Handle, ActorInfo, ActivationInfo, true);
        return;
    }

    // Check cooldown
    if (Player->GetRadianceCooldownRemaining() > 0.0f)
    {
        CancelAbility(Handle, ActorInfo, ActivationInfo, true);
        return;
    }

    const float RadianceFaithCost = Player->RadianceFaithCost;
    if (Player->GetFaith() < RadianceFaithCost)
    {
        CancelAbility(Handle, ActorInfo, ActivationInfo, true);
        return;
    }

    if (!CommitMiracle(ActorInfo))
    {
        CancelAbility(Handle, ActorInfo, ActivationInfo, true);
        return;
    }

    // AOE damage to nearby enemies
    const float Radius = Player->RadianceRadius;
    const float Damage = Player->RadianceDamage;
    const float PoiseDamage = Player->RadiancePoiseDamage;

    UWorld* World = Player->GetWorld();
    if (World != nullptr)
    {
        for (TActorIterator<ANazareneEnemyCharacter> It(World); It; ++It)
        {
            ANazareneEnemyCharacter* Enemy = *It;
            if (Enemy == nullptr || Enemy->IsRedeemed())
            {
                continue;
            }
            const FVector ToEnemy = Enemy->GetActorLocation() - Player->GetActorLocation();
            if (ToEnemy.Size() <= Radius)
            {
                Enemy->ReceiveCombatHit(Damage, PoiseDamage, Player);
                Enemy->ApplyKnockback(ToEnemy.GetSafeNormal2D(), 450.0f);
            }
        }
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
