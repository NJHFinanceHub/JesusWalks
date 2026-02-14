#include "GE_NazareneMiracleCost.h"

#include "NazareneAttributeSet.h"

UGE_NazareneMiracleCost::UGE_NazareneMiracleCost()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;

    FGameplayModifierInfo& FaithCostModifier = Modifiers.AddDefaulted_GetRef();
    FaithCostModifier.Attribute = UNazareneAttributeSet::GetFaithAttribute();
    FaithCostModifier.ModifierOp = EGameplayModOp::Additive;

    FSetByCallerFloat FaithCostMagnitude;
    FaithCostMagnitude.DataTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Data.Miracle.FaithCost")), false);
    FaithCostModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FaithCostMagnitude);
}
