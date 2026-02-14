#pragma once

#include "CoreMinimal.h"
#include "NazareneGameplayAbility.h"
#include "GA_NazareneRadiance.generated.h"

UCLASS()
class THENAZARENEAAA_API UGA_NazareneRadiance : public UNazareneGameplayAbility
{
    GENERATED_BODY()

public:
    UGA_NazareneRadiance();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
