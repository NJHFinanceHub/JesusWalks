#include "GA_NazareneBlessing.h"

#include "NazareneAttributeSet.h"
#include "NazarenePlayerCharacter.h"
#include "GameplayTagsManager.h"

UGA_NazareneBlessing::UGA_NazareneBlessing()
{
    FaithCost = 22.0f;
    CooldownDuration = 14.0f;
    CooldownTag = FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Miracle.Blessing"));
}

void UGA_NazareneBlessing::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    ANazarenePlayerCharacter* Player = GetNazarenePlayerCharacter(ActorInfo);
    UNazareneAttributeSet* Attributes = GetNazareneAttributeSet(ActorInfo);

    if (Player == nullptr || Attributes == nullptr)
    {
        CancelAbility(Handle, ActorInfo, ActivationInfo, true);
        return;
    }

    // Check miracle unlock
    if (!Player->IsMiracleUnlocked(FName(TEXT("blessing"))))
    {
        Player->SetContextHint(TEXT("Blessing miracle not yet unlocked."));
        CancelAbility(Handle, ActorInfo, ActivationInfo, true);
        return;
    }

    // Check cooldown
    if (Player->GetBlessingCooldownRemaining() > 0.0f)
    {
        CancelAbility(Handle, ActorInfo, ActivationInfo, true);
        return;
    }

    const float BlessingFaithCost = Player->BlessingFaithCost;
    if (Player->GetFaith() < BlessingFaithCost)
    {
        CancelAbility(Handle, ActorInfo, ActivationInfo, true);
        return;
    }

    if (!CommitMiracle(ActorInfo))
    {
        CancelAbility(Handle, ActorInfo, ActivationInfo, true);
        return;
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
