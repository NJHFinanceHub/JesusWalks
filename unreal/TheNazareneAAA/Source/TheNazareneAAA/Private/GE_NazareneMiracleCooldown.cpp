#include "GE_NazareneMiracleCooldown.h"

UGE_NazareneMiracleCooldown::UGE_NazareneMiracleCooldown()
{
    DurationPolicy = EGameplayEffectDurationType::HasDuration;

    FSetByCallerFloat CooldownMagnitude;
    CooldownMagnitude.DataTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Data.Miracle.Cooldown")), false);
    DurationMagnitude = FGameplayEffectModifierMagnitude(CooldownMagnitude);
}
