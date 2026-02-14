#pragma once

#include "CoreMinimal.h"
#include "NazareneGameplayAbility.h"
#include "GA_NazareneBlessing.generated.h"

UCLASS()
class THENAZARENEAAA_API UGA_NazareneBlessing : public UNazareneGameplayAbility
{
    GENERATED_BODY()

public:
    UGA_NazareneBlessing();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
