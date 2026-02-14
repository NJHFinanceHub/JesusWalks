#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "NazareneAbilitySystemComponent.generated.h"

UCLASS(BlueprintType)
class THENAZARENEAAA_API UNazareneAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void InitializeForActor(AActor* InOwnerActor, AActor* InAvatarActor);

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void GrantDefaultAbilities();
};
