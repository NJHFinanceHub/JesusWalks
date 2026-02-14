#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "NazareneAttributeSet.generated.h"

#define NAZARENE_ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS(BlueprintType)
class THENAZARENEAAA_API UNazareneAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

public:
    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Health;
    NAZARENE_ATTRIBUTE_ACCESSORS(UNazareneAttributeSet, Health);

    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData MaxHealth;
    NAZARENE_ATTRIBUTE_ACCESSORS(UNazareneAttributeSet, MaxHealth);

    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Stamina;
    NAZARENE_ATTRIBUTE_ACCESSORS(UNazareneAttributeSet, Stamina);

    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData MaxStamina;
    NAZARENE_ATTRIBUTE_ACCESSORS(UNazareneAttributeSet, MaxStamina);

    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Faith;
    NAZARENE_ATTRIBUTE_ACCESSORS(UNazareneAttributeSet, Faith);
};
