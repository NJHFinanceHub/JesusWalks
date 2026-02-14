#include "GA_NazareneHeal.h"

#include "NazareneAttributeSet.h"
#include "NazarenePlayerCharacter.h"

UGA_NazareneHeal::UGA_NazareneHeal()
{
    FaithCost = 18.0f;
    CooldownDuration = 6.5f;
}

void UGA_NazareneHeal::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    ANazarenePlayerCharacter* Player = GetNazarenePlayerCharacter(ActorInfo);
    UNazareneAttributeSet* Attributes = GetNazareneAttributeSet(ActorInfo);

    if (Player == nullptr || Attributes == nullptr)
    {
        CancelAbility(Handle, ActorInfo, ActivationInfo, true);
        return;
    }

    // Read tunable values from the player character
    const float HealFaithCost = Player->HealFaithCost;
    const float HealCooldown = Player->HealCooldown;
    const float HealAmt = Player->HealAmount;

    // Check cooldown and health prerequisites
    if (Player->GetHealCooldownRemaining() > 0.0f)
    {
        CancelAbility(Handle, ActorInfo, ActivationInfo, true);
        return;
    }

    if (Player->GetHealth() >= Player->GetMaxHealth())
    {
        CancelAbility(Handle, ActorInfo, ActivationInfo, true);
        return;
    }

    if (Player->GetFaith() < HealFaithCost)
    {
        CancelAbility(Handle, ActorInfo, ActivationInfo, true);
        return;
    }

    // Deduct faith
    Player->AddFaith(-HealFaithCost);

    // Heal: directly set via the public setter pattern matching existing code
    const float NewHealth = FMath::Min(Player->GetMaxHealth(), Player->GetHealth() + HealAmt);
    Attributes->SetHealth(NewHealth);

    // Display the heal number
    Player->DisplayDamageNumber(Player->GetActorLocation() + FVector(0.0f, 0.0f, 130.0f), HealAmt, ENazareneDamageNumberType::Heal);

    EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
