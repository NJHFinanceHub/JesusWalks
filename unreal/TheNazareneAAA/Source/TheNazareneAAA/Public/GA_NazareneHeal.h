#pragma once

#include "CoreMinimal.h"
#include "NazareneGameplayAbility.h"
#include "GA_NazareneHeal.generated.h"

UCLASS()
class THENAZARENEAAA_API UGA_NazareneHeal : public UNazareneGameplayAbility
{
    GENERATED_BODY()

public:
    UGA_NazareneHeal();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
