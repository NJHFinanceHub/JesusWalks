#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "NazareneGameplayAbility.generated.h"

class ANazarenePlayerCharacter;
class UNazareneAttributeSet;

UCLASS(Abstract)
class THENAZARENEAAA_API UNazareneGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UNazareneGameplayAbility();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miracle")
    float FaithCost = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miracle")
    float CooldownDuration = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miracle")
    FGameplayTag CooldownTag;

    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

protected:
    bool CommitMiracle(const FGameplayAbilityActorInfo* ActorInfo) const;
    ANazarenePlayerCharacter* GetNazarenePlayerCharacter(const FGameplayAbilityActorInfo* ActorInfo) const;
    UNazareneAttributeSet* GetNazareneAttributeSet(const FGameplayAbilityActorInfo* ActorInfo) const;
};
