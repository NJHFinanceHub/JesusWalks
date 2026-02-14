#include "GE_NazareneMiracleCost.h"

#include "NazareneAttributeSet.h"

UGE_NazareneMiracleCost::UGE_NazareneMiracleCost()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;

    FGameplayModifierInfo& FaithCostModifier = Modifiers.AddDefaulted_GetRef();
    FaithCostModifier.Attribute = UNazareneAttributeSet::GetFaithAttribute();
    FaithCostModifier.ModifierOp = EGameplayModOp::Additive;

    FSetByCallerFloat FaithCostMagnitude;
    FaithCostMagnitude.DataTag = FGameplayTag::RequestGameplayTag(TEXT("Data.Miracle.FaithCost"));
    FaithCostModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FaithCostMagnitude);
}
