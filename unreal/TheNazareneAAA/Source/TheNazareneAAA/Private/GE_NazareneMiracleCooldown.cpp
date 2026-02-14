#include "GE_NazareneMiracleCooldown.h"

UGE_NazareneMiracleCooldown::UGE_NazareneMiracleCooldown()
{
    DurationPolicy = EGameplayEffectDurationType::HasDuration;
    DurationMagnitude = FGameplayEffectModifierMagnitude(FSetByCallerFloat(FGameplayTag::RequestGameplayTag(TEXT("Data.Miracle.Cooldown"))));
}
