#include "NazareneHUD.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "NazarenePlayerCharacter.h"

void ANazareneHUD::DrawHUD()
{
    Super::DrawHUD();

    if (Canvas == nullptr)
    {
        return;
    }

    ANazarenePlayerCharacter* Player = Cast<ANazarenePlayerCharacter>(GetOwningPawn());
    if (Player == nullptr)
    {
        return;
    }

    const float HealthRatio = Player->GetMaxHealth() > 0.0f ? Player->GetHealth() / Player->GetMaxHealth() : 0.0f;
    const float StaminaRatio = Player->GetMaxStamina() > 0.0f ? Player->GetStamina() / Player->GetMaxStamina() : 0.0f;

    DrawText(TEXT("The Nazarene - Unreal Migration"), FLinearColor(0.95f, 0.90f, 0.78f), 40.0f, 30.0f, GEngine->GetSmallFont());
    DrawRect(FLinearColor(0.08f, 0.08f, 0.06f, 0.7f), 24.0f, 60.0f, 430.0f, 180.0f);
    DrawText(FString::Printf(TEXT("Health: %.0f / %.0f"), Player->GetHealth(), Player->GetMaxHealth()), FLinearColor::White, 40.0f, 76.0f, GEngine->GetSmallFont());
    DrawRect(FLinearColor(0.20f, 0.16f, 0.16f, 1.0f), 40.0f, 104.0f, 360.0f, 14.0f);
    DrawRect(FLinearColor(0.84f, 0.25f, 0.21f, 1.0f), 40.0f, 104.0f, 360.0f * FMath::Clamp(HealthRatio, 0.0f, 1.0f), 14.0f);

    DrawText(FString::Printf(TEXT("Stamina: %.0f / %.0f"), Player->GetStamina(), Player->GetMaxStamina()), FLinearColor::White, 40.0f, 126.0f, GEngine->GetSmallFont());
    DrawRect(FLinearColor(0.16f, 0.20f, 0.16f, 1.0f), 40.0f, 154.0f, 360.0f, 14.0f);
    DrawRect(FLinearColor(0.30f, 0.78f, 0.34f, 1.0f), 40.0f, 154.0f, 360.0f * FMath::Clamp(StaminaRatio, 0.0f, 1.0f), 14.0f);

    DrawText(FString::Printf(TEXT("Faith: %.0f"), Player->GetFaith()), FLinearColor(0.93f, 0.86f, 0.54f), 40.0f, 174.0f, GEngine->GetSmallFont());
    DrawText(FString::Printf(TEXT("Lock-On: %s"), *Player->GetLockTargetName()), FLinearColor(0.88f, 0.88f, 0.88f), 40.0f, 194.0f, GEngine->GetSmallFont());
    DrawText(Player->GetContextHint(), FLinearColor(0.92f, 0.82f, 0.66f), 40.0f, 214.0f, GEngine->GetSmallFont());

    DrawRect(FLinearColor(0.08f, 0.08f, 0.06f, 0.7f), Canvas->SizeX - 460.0f, 30.0f, 430.0f, 130.0f);
    DrawText(RegionName, FLinearColor(0.95f, 0.90f, 0.78f), Canvas->SizeX - 440.0f, 46.0f, GEngine->GetSmallFont());
    DrawText(Objective, FLinearColor(0.87f, 0.83f, 0.72f), Canvas->SizeX - 440.0f, 76.0f, GEngine->GetSmallFont());

    if (MessageTimer > 0.0f)
    {
        MessageTimer = FMath::Max(MessageTimer - GetWorld()->DeltaTimeSeconds, 0.0f);
        DrawText(TransientMessage, FLinearColor(0.95f, 0.90f, 0.78f), Canvas->SizeX * 0.28f, 20.0f, GEngine->GetSmallFont());
    }

    const FString Controls = TEXT("WASD Move | LMB Light | RMB Heavy | Space Dodge | Shift Block | F Parry | Q Lock | R Heal | 1 Blessing | 2 Radiance | E Pray | F1-F3 Save | F5-F7 Load");
    DrawText(Controls, FLinearColor(0.90f, 0.85f, 0.70f), 24.0f, Canvas->SizeY - 38.0f, GEngine->GetSmallFont());
}

void ANazareneHUD::SetRegionName(const FString& InRegionName)
{
    RegionName = InRegionName;
}

void ANazareneHUD::SetObjective(const FString& InObjective)
{
    Objective = InObjective;
}

void ANazareneHUD::ShowMessage(const FString& InMessage, float Duration)
{
    TransientMessage = InMessage;
    MessageTimer = Duration;
}

