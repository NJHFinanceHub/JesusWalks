#include "NazareneGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "GE_NazareneMiracleCooldown.h"
#include "GE_NazareneMiracleCost.h"
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

    if (CooldownTag.IsValid() && ActorInfo != nullptr && ActorInfo->AbilitySystemComponent != nullptr)
    {
        if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(CooldownTag))
        {
            return false;
        }
    }

    return true;
}

bool UNazareneGameplayAbility::CommitMiracle(const FGameplayAbilityActorInfo* ActorInfo) const
{
    if (ActorInfo == nullptr || ActorInfo->AbilitySystemComponent == nullptr)
    {
        return false;
    }

    UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
    if (ASC == nullptr)
    {
        return false;
    }

    FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();

    FGameplayEffectSpecHandle CostSpec = ASC->MakeOutgoingSpec(UGE_NazareneMiracleCost::StaticClass(), 1.0f, EffectContext);
    if (!CostSpec.IsValid())
    {
        return false;
    }
    CostSpec.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TEXT("Data.Miracle.FaithCost")), -FMath::Abs(FaithCost));
    ASC->ApplyGameplayEffectSpecToSelf(*CostSpec.Data.Get());

    if (CooldownDuration > 0.0f && CooldownTag.IsValid())
    {
        FGameplayEffectSpecHandle CooldownSpec = ASC->MakeOutgoingSpec(UGE_NazareneMiracleCooldown::StaticClass(), 1.0f, EffectContext);
        if (!CooldownSpec.IsValid())
        {
            return false;
        }

        CooldownSpec.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TEXT("Data.Miracle.Cooldown")), CooldownDuration);
        CooldownSpec.Data->DynamicGrantedTags.AddTag(CooldownTag);
        ASC->ApplyGameplayEffectSpecToSelf(*CooldownSpec.Data.Get());
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
