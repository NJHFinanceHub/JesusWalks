#include "NazareneGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "NazareneAttributeSet.h"
#include "NazarenePlayerCharacter.h"

UNazareneGameplayAbility::UNazareneGameplayAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UNazareneGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
    {
        return false;
    }

    const UNazareneAttributeSet* Attributes = GetNazareneAttributeSet(ActorInfo);
    if (Attributes == nullptr)
    {
        return false;
    }

    if (Attributes->GetFaith() < FaithCost)
    {
        return false;
    }

    return true;
}

ANazarenePlayerCharacter* UNazareneGameplayAbility::GetNazarenePlayerCharacter(const FGameplayAbilityActorInfo* ActorInfo) const
{
    if (ActorInfo == nullptr || !ActorInfo->AvatarActor.IsValid())
    {
        return nullptr;
    }

    return Cast<ANazarenePlayerCharacter>(ActorInfo->AvatarActor.Get());
}

UNazareneAttributeSet* UNazareneGameplayAbility::GetNazareneAttributeSet(const FGameplayAbilityActorInfo* ActorInfo) const
{
    if (ActorInfo == nullptr || ActorInfo->AbilitySystemComponent == nullptr)
    {
        return nullptr;
    }

    return const_cast<UNazareneAttributeSet*>(ActorInfo->AbilitySystemComponent->GetSet<UNazareneAttributeSet>());
}
