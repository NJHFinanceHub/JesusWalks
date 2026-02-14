#include "GE_NazareneMiracleCooldown.h"

UGE_NazareneMiracleCooldown::UGE_NazareneMiracleCooldown()
{
    DurationPolicy = EGameplayEffectDurationType::HasDuration;

    FSetByCallerFloat CooldownMagnitude;
    CooldownMagnitude.DataTag = FGameplayTag::RequestGameplayTag(TEXT("Data.Miracle.Cooldown"));
    DurationMagnitude = FGameplayEffectModifierMagnitude(CooldownMagnitude);
}
