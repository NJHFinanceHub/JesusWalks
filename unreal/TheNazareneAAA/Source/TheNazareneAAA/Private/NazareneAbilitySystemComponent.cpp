#include "NazareneAbilitySystemComponent.h"

#include "GA_NazareneHeal.h"
#include "GA_NazareneBlessing.h"
#include "GA_NazareneRadiance.h"

void UNazareneAbilitySystemComponent::InitializeForActor(AActor* InOwnerActor, AActor* InAvatarActor)
{
    if (InOwnerActor == nullptr)
    {
        return;
    }

    InitAbilityActorInfo(InOwnerActor, InAvatarActor != nullptr ? InAvatarActor : InOwnerActor);
}

void UNazareneAbilitySystemComponent::GrantDefaultAbilities()
{
    if (GetOwner() == nullptr)
    {
        return;
    }

    GiveAbility(FGameplayAbilitySpec(UGA_NazareneHeal::StaticClass(), 1, INDEX_NONE, this));
    GiveAbility(FGameplayAbilitySpec(UGA_NazareneBlessing::StaticClass(), 1, INDEX_NONE, this));
    GiveAbility(FGameplayAbilitySpec(UGA_NazareneRadiance::StaticClass(), 1, INDEX_NONE, this));
}
